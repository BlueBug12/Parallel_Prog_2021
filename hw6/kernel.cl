__kernel void convolution(
    __global float *src,
    __global float *dest,
    __constant float *filter,
    int w,
    int h,
    int filter_size) {
    float result = 0;
    const int ix = get_global_id(0);
    const int iy = get_global_id(1);
    const int hw = filter_size >> 1;
    for(int k = -hw;k<=hw;++k){
        int x_idx = ix + k;
        for(int l = -hw;l<=hw;++l){
            int y_idx = iy + l;
            if(x_idx>=0 && x_idx<w && y_idx>=0 && y_idx<h){
                result += filter[(l+hw)*filter_size+(k+hw)]*src[y_idx*w+x_idx];
            }
        }
    }
    dest[iy*w+ix] = result;
}
