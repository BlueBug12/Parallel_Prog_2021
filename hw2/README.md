###### tags: `PP`

# Parallel Programming HW2

## Part 1:
### Execution:
```
make
./pi.out thread_num toss_num
```


## Part 2:
### Q1

![](https://i.imgur.com/eByUcZ8.png)

![](https://i.imgur.com/wtXGvMB.png)




We can see that at the 3-thread data-point for **VIEW 1**, the speedup is lower than two-thread data-point, and the efficiencies of 3-thread and 4-threas data-point is also not good as 2-threads one. But for **VIEW 2**, the same program generates a more linear result. These two example tell us that the speedup result depends on the input case. I think the reason is because of the function `mandel` called by each thread has a `break` in the for loop. It means that the iteration number is not fixed, depending on the input. As a result, it may lead to a load unbalance issue. I guess that the data-point in **VIEW 1** with 3 threads is where this problem happens.

```cpp=
static inline int mandel(float c_re, float c_im, int count)
{
  float z_re = c_re, z_im = c_im;
  int i;
  for (i = 0; i < count; ++i)
  {
    if (z_re * z_re + z_im * z_im > 4.f)
      break;

    float new_re = z_re * z_re - z_im * z_im;
    float new_im = 2.f * z_re * z_im;
    z_re = c_re + new_re;
    z_im = c_im + new_im;
  }
  return i;
}
```
### Q2
After adding `clock` in the `workerThreadStart` function, we can measure the amount of time each thread requires to complete its work. Just see the result below.
For VIEW 1, terminal shows:
```c
//thread number = 1
Thread 0 costs 461.362 ms.

//thread number = 2
Thread 0 costs 237.599 ms.
Thread 1 costs 241.013 ms.

//thread number = 3
Thread 0 costs 97.2102 ms.
Thread 2 costs 98.194 ms.
Thread 1 costs 287.566 ms. //unbalance loading

//thread number = 4
Thread 0 costs 49.878 ms.
Thread 3 costs 50.5353 ms. //unbalance loading
Thread 2 costs 198.619 ms. //unbalance loading
Thread 1 costs 199.356 ms. 
```

For VIEW 2, terminal shows:
```c
//thread number = 1
Thread 0 costs 291.487 ms.

//thread number = 2
Thread 0 costs 176.049 ms.
Thread 1 costs 124.015 ms.

//thread number = 3
Thread 2 costs 80.6475 ms.
Thread 1 costs 87.6075 ms.
Thread 0 costs 132.798 ms.
    
//thread number = 4
Thread 3 costs 65.8315 ms.
Thread 2 costs 69.8483 ms.
Thread 1 costs 71.2349 ms.
Thread 0 costs 115.166 ms.
```

As we can see, for VIEW 1, the 3-thread case suffers from a serious unblance loading for each thread, and so does the 4-thread case. As for VIEW 2, the load unbalance issue is not as significant as VIEW 1. So it proves the hypothesis we made befoe is true.

### Q3
Because I found that there are some blocks of the image that took more time than others, I thought that if each row segment can be distributed with an interleaving way, the load-unblanced problem should be solved. 
Just see the following example(assume number of thread = 4):
```
image row 0 -> manipulated by thread 0
image row 1 -> manipulated by thread 1
image row 2 -> manipulated by thread 2
image row 3 -> manipulated by thread 3
image row 4 -> manipulated by thread 0
image row 5 -> manipulated by thread 1
image row 6 -> manipulated by thread 2
image row 7 -> manipulated by thread 3
.
.
.
```
And the corresponding code should be:
```c++=
void workerThreadStart(WorkerArgs *const args)
{
    float x0 = args->x0;
    float x1 = args->x1;
    float y0 = args->y0;
    float y1 = args->y1;
    int segment = args->numThreads;
    int startRow = args->threadId;
    for(;startRow<(int)args->height;startRow+=segment){
        mandelbrotSerial(x0,y0,x1,y1,args->width,args->height,startRow,1,args->maxIterations,args->output);
    }
}
```

Although this method seems very intuitive and easy, the result shows that the efficiency of the program doesn't degrade a lot when the number of thread increases.
![](https://i.imgur.com/YPxQ0pP.png)
![](https://i.imgur.com/LODXbo1.png)

### Q4
Experiment result:
```shell
./mandelbrot -t 8 -v 1
[mandelbrot serial]:            [463.676] ms
Wrote image file mandelbrot-serial.ppm
[mandelbrot thread]:            [125.718] ms
Wrote image file mandelbrot-thread.ppm
                                (3.69x speedup from 8 threads)
                                
./mandelbrot -t 8 -v 2
[mandelbrot serial]:            [289.921] ms
Wrote image file mandelbrot-serial.ppm
[mandelbrot thread]:            [76.615] ms
Wrote image file mandelbrot-thread.ppm
                                (3.78x speedup from 8 threads)

```

We can see that the performance doesn't become greater with 8 threads. Actually, when the number of thread is more than 4, there seems no significant improvement. I think it is because the workstation server provides only 4 cores 4 threads, the speedup achieved by multi-threading has a limit of up to 4 times.
