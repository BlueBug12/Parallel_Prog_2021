make clean >/dev/null
make >/dev/null
echo "kernel.cu v=1 iter=1000"
./mandelbrot -v 1 -i 1000
echo "kernel.cu v=1 iter=10000"
./mandelbrot -v 1 -i 10000
echo "kernel.cu v=1 iter=100000"
./mandelbrot -v 1 -i 100000
echo "kernel.cu v=2 iter=1000"
./mandelbrot -v 2 -i 1000
echo "kernel.cu v=2 iter=10000"
./mandelbrot -v 2 -i 10000
echo "kernel.cu v=2 iter=100000"
./mandelbrot -v 2 -i 100000

make clean >/dev/null
make -f Makefile2.mk >/dev/null
echo "kernel2.cu v=1 iter=1000"
./mandelbrot -v 1 -i 1000
echo "kernel2.cu v=1 iter=10000"
./mandelbrot -v 1 -i 10000
echo "kernel2.cu v=1 iter=100000"
./mandelbrot -v 1 -i 100000
echo "kernel2.cu v=2 iter=1000"
./mandelbrot -v 2 -i 1000
echo "kernel2.cu v=2 iter=10000"
./mandelbrot -v 2 -i 10000
echo "kernel2.cu v=2 iter=100000"
./mandelbrot -v 2 -i 100000


make clean >/dev/null
make -f Makefile3.mk >/dev/null
echo "kernel3.cu v=1 iter=1000"
./mandelbrot -v 1 -i 1000
echo "kernel3.cu v=1 iter=10000"
./mandelbrot -v 1 -i 10000
echo "kernel3.cu v=1 iter=100000"
./mandelbrot -v 1 -i 100000
echo "kernel3.cu v=2 iter=1000"
./mandelbrot -v 2 -i 1000
echo "kernel3.cu v=2 iter=10000"
./mandelbrot -v 2 -i 10000
echo "kernel3.cu v=2 iter=100000"
./mandelbrot -v 2 -i 100000

