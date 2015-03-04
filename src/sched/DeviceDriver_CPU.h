
#include "DeviceDriver.h"

#ifndef _DEVICE_DRIVER_CPU_H
#define _DEVICE_DRIVER_CPU_H

class CPUDriver : public DeviceDriver{
public:

  void memcpy(DeviceMemoryPointer dst, DeviceMemoryPointer src){
#ifdef _DO_ASSERT
    assert(dst.type==DEVICEMEMORY_LOCAL_RAM);
    assert(src.type==DEVICEMEMORY_LOCAL_RAM);
    assert(dst.size_in_byte == src.size_in_byte);
#endif
    char *s1 = (char*) dst.ptr;
    const char *s2 = (const char*) src.ptr;
    size_t n = dst.size_in_byte;
    for(; 0<n; --n)*s1++ = *s2++;
  }

  void memset(DeviceMemoryPointer dst, const char value){
#ifdef _DO_ASSERT
    assert(dst.type==DEVICEMEMORY_LOCAL_RAM);
#endif
    char *s1 = (char*) dst.ptr;
    size_t n = dst.size_in_byte;
    for(; 0<n; --n)*s1++ = value;
  }

    void smath_axpy(const float alpha, DeviceMemoryPointer X, DeviceMemoryPointer Y)  { 
#ifdef _DO_ASSERT
    assert(X.type==DEVICEMEMORY_LOCAL_RAM);
    assert(Y.type==DEVICEMEMORY_LOCAL_RAM);
    assert(X.size_in_byte==Y.size_in_byte);
#endif
      cblas_saxpy(X.size_in_byte/sizeof(float), alpha, (float *) X.ptr, 1, (float *) Y.ptr, 1); 
    }

  void sapply(DeviceMemoryPointer dst, const size_t n_element, std::function<void(float&)> func){
#ifdef _DO_ASSERT
    assert(dst.type==DEVICEMEMORY_LOCAL_RAM);
    assert(dst.size_in_byte == elemnent_size * n_element);
#endif
    float * p = (float*) dst.ptr;
    for(size_t i=0;i<n_element;i++){
      func(*(p++));
    }
  }

#ifdef _USE_OPENBLAS
  	void smath_axpby(const float alpha, DeviceMemoryPointer X, const float beta, DeviceMemoryPointer Y) { 
#ifdef _DO_ASSERT
  		assert(X.size_in_byte == Y.size_in_byte);
  		assert(X.size_in_byte % sizeof(float) == 0);
#endif
    	cblas_saxpby(X.size_in_byte/sizeof(float), alpha, (float*)X.ptr, 1, beta, (float*) Y.ptr, 1); 
    }
    void set_num_threads(const int nThreads) { openblas_set_num_threads(nThreads); }
#elif _USE_ATLAS
  	void smath_axpby(const float alpha, DeviceMemoryPointer X, const float beta, DeviceMemoryPointer Y) { 
#ifdef _DO_ASSERT
  		assert(X.size_in_byte == Y.size_in_byte);
  		assert(X.size_in_byte % sizeof(float) == 0);
#endif
    	catlas_saxpby(X.size_in_byte/sizeof(float), alpha, (float*)X.ptr, 1, beta, (float*) Y.ptr, 1); 
    }
    void set_num_threads(const int nThreads) {  set_num_threads(nThreads); }
#elif _VANILLA_BLAS
    #warning "[PERFORMANCE WARNING] Using hand-written BLAS calls. Hope you have a good compiler!"
  	void smath_axpby(const float alpha, DeviceMemoryPointer X, const float beta, DeviceMemoryPointer Y) { 
#ifdef _DO_ASSERT
  		assert(X.size_in_byte == Y.size_in_byte);
  		assert(X.size_in_byte % sizeof(float) == 0);
#endif
    	const int N = X.size_in_byte/sizeof(float);
    	float * _X = X.ptr;
    	float * _Y = Y.ptr;
    	for(int i = N; i > 0; _X++, _Y++, --i) {
			*Y = alpha**_X + beta* *_Y;
      	}
    }
    void set_num_threads(const int nThreads) {   }
#else
	#error "Select a BLAS framework."
#endif

  std::function<void(float&)> srand_uni(float lower, float upper){
    mt19937 *gen = new mt19937(rd());
    uniform_real_distribution<float> *uni = new 
      uniform_real_distribution<float>(lower, upper);
    return [=](float & b) { b = (*uni)(*gen); };
  }

  std::function<void(float&)> srand_bern(float p){
    mt19937 *gen = new mt19937(rd());
    bernoulli_distribution * bern = new bernoulli_distribution(p);
    return [=](float & b) { b = (*bern)(*gen); };
  }

  std::function<void(float&)> srand_gaussian(float mean, float std_dev){
    mt19937 *gen = new mt19937(rd());
    normal_distribution<float> *gaussian = new
      normal_distribution<float>(mean, std_dev);
    return [=](float & b) { b = (*gaussian)(*gen); };
  }

  using DeviceDriver::sinitialize_xavier;

  using DeviceDriver::bernoulli_initialize;

  using DeviceDriver::gaussian_initialize;

  using DeviceDriver::constant_initialize;

  using DeviceDriver::smath_apply_grad;

private:

  random_device rd;
};

#endif

