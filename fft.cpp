#include <stdio.h>
#include <stdlib.h>
#include <complex>
#include <vector>

const float pi = 3.1415;

std::vector<std::complex<float> > fft(std::vector<std::complex<float> > x) {
  if (x.size() == 1) {
    return x;
  }
  auto even = std::vector<std::complex<float> >();
  auto odd = std::vector<std::complex<float> >();
  for (int i = 0; i < x.size(); i++) {
    if (i % 2 == 0) {
      even.push_back(x[i]);
    } else {
      odd.push_back(x[i]);
    }
  }
  even = fft(even);
  odd = fft(odd);
  for (int i = 0; i < even.size(); i++) {
    auto t = even[i];
    even[i] = t + std::exp(std::complex<float>(0, -2*pi*i/x.size())) * odd[i];
    odd[i] = t - std::exp(std::complex<float>(0, -2*pi*i/x.size())) * odd[i];
  }
  for (auto k : odd) {
    even.push_back(k);
  }
  return even;
}

std::complex<float> img[512 * 512];

int main() {
  FILE* f = fopen("./lena.ppm", "r");
  char buf[512*3 + 1];
  fgets(buf, 15, f);
  fgets(buf, 15, f);
  fgets(buf, 15, f);
  printf("P2\n512 512\n255\n");
  for (int i = 0; i < 512; i++) {
    fread(buf, sizeof(char), 512*3, f);
    for (int j = 0; j/3 < 512; j+=3) {
      img[i * 512 + j / 3] = std::complex<float>(((unsigned char) buf[j]), 0);
    }
  }
  for (int i = 0; i < 512; i++) {
    std::vector<std::complex<float> > f1;
    for (int j = 0; j < 512; j++) {
      f1.push_back(img[i * 512 + j]);
    }
    f1 = fft(f1);
    for (int j = 0; j < 512; j++) {
      img[i * 512 + j] = f1[j];
    }
  }
  for (int i = 0; i < 512; i++) {
    std::vector<std::complex<float> > f1;
    for (int j = 0; j < 512; j++) {
      f1.push_back(img[512 * j + i]);
    }
    f1 = fft(f1);
    for (int j = 0; j < 512; j++) {
      img[j * 512 + i] = f1[j];
    }
  }
  float max = 0;
  for (int i = 0; i < 512; i++) {
    for (int j = 0; j < 512; j++) {
      max = std::abs(img[i * 512 + j]) > max ? std::abs(img[i * 512 + j]) : max;
    }
  }
  for (int i = 0; i < 512; i++) {
    for (int j = 0; j < 512; j++) {
      int oi = (i + 256) % 512;
      int oj = (j + 256) % 512;
      int c = std::log(std::abs(img[oi * 512 + oj])) / std::log(max) * 255;
      // int c = std::abs(img[oi * 512 + oj]) / max * 255;
      printf("%d ", c);
    }
    printf("\n");
  }
  return 0;
}
