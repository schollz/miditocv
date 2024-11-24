#ifndef LIB_SPECTRA_H
#define LIB_SPECTRA_H 1

#include <math.h>
#include <stdint.h>

#define SPECTRA_M 400       // Image width
#define SPECTRA_N 50        // Image height
#define SPECTRA_GAMMA 0.8f  // Gamma correction factor

// Gamma correction function
float gammaCorrect(float value) { return powf(value, SPECTRA_GAMMA); }

// Map normalized value (0.0 to 1.0) to RGB
void normalizedToRGB(float normalized, float *r, float *g, float *b) {
  float wl = 380.0f +
             (normalized *
              400.0f);  // Map normalized value to wavelength (380 nm to 780 nm)

  if (wl >= 380.0f && wl <= 440.0f) {
    *r = -(wl - 440.0f) / (440.0f - 380.0f);
    *g = 0.0f;
    *b = 1.0f;
  } else if (wl > 440.0f && wl <= 490.0f) {
    *r = 0.0f;
    *g = (wl - 440.0f) / (490.0f - 440.0f);
    *b = 1.0f;
  } else if (wl > 490.0f && wl <= 510.0f) {
    *r = 0.0f;
    *g = 1.0f;
    *b = -(wl - 510.0f) / (510.0f - 490.0f);
  } else if (wl > 510.0f && wl <= 580.0f) {
    *r = (wl - 510.0f) / (580.0f - 510.0f);
    *g = 1.0f;
    *b = 0.0f;
  } else if (wl > 580.0f && wl <= 645.0f) {
    *r = 1.0f;
    *g = -(wl - 645.0f) / (645.0f - 580.0f);
    *b = 0.0f;
  } else if (wl > 645.0f && wl <= 780.0f) {
    *r = 1.0f;
    *g = 0.0f;
    *b = 0.0f;
  } else {
    *r = *g = *b = 0.0f;
  }

  // Intensity correction near vision limits
  float sss = 1.0f;
  if (wl > 700.0f) {
    sss = 0.3f + 0.7f * (780.0f - wl) / (780.0f - 700.0f);
  } else if (wl < 420.0f) {
    sss = 0.3f + 0.7f * (wl - 380.0f) / (420.0f - 380.0f);
  }

  *r *= sss;
  *g *= sss;
  *b *= sss;
}

// Convert normalized value (0.0 to 1.0) to uint8 RGB with gamma correction
void RGB_Spectra_ToUint8(float x, uint8_t *r, uint8_t *g, uint8_t *b) {
  float r1, g1, b1;
  normalizedToRGB(x, &r1, &g1, &b1);

  // Apply gamma correction
  r1 = gammaCorrect(r1);
  g1 = gammaCorrect(g1);
  b1 = gammaCorrect(b1);

  // Convert to uint8
  *r = (uint8_t)roundf(r1 * 255.0f);
  *g = (uint8_t)roundf(g1 * 255.0f);
  *b = (uint8_t)roundf(b1 * 255.0f);
}

#endif
