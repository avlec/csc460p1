#ifndef EXPONENTIALAVERAGINGFILTER_H
#define EXPONENTIALAVERAGINGFILTER_H

#define DEAD_BAND_VARIANCE 10

int DEAD_BAND_MIDDLE_X = 512;
int DEAD_BAND_MIDDLE_Y = 512;

struct float_lowpass_filter {
  float alpha;
  float y_prev;
};
  
float flpf_getx(int x, struct float_lowpass_filter filter) {
  if((x > (DEAD_BAND_MIDDLE_X - DEAD_BAND_VARIANCE)) && (x < (DEAD_BAND_MIDDLE_X + DEAD_BAND_VARIANCE)))
    x = DEAD_BAND_MIDDLE_X;
  filter.y_prev = filter.alpha*x + (1.0 - filter.alpha)*filter.y_prev;
  return filter.y_prev;
}

float flpf_gety(int x, struct float_lowpass_filter filter) {
  if((x > (DEAD_BAND_MIDDLE_Y - DEAD_BAND_VARIANCE)) && (x < (DEAD_BAND_MIDDLE_Y + DEAD_BAND_VARIANCE)))
    x = DEAD_BAND_MIDDLE_Y;
  filter.y_prev = filter.alpha*x + (1.0 - filter.alpha)*filter.y_prev;
  return filter.y_prev;
}

struct float_lowpass_filter GLOBAL_X_LOWPASS_FILTER;
struct float_lowpass_filter GLOBAL_Y_LOWPASS_FILTER;

#define ANALOGINITX(A) GLOBAL_X_LOWPASS_FILTER.alpha = A; GLOBAL_X_LOWPASS_FILTER.y_prev = analogRead(X_PIN)
#define ANALOGREADX() flpf_getx(analogRead(X_PIN), GLOBAL_X_LOWPASS_FILTER)
#define ANALOGINITY(A) GLOBAL_Y_LOWPASS_FILTER.alpha = A; GLOBAL_Y_LOWPASS_FILTER.y_prev = analogRead(Y_PIN)
#define ANALOGREADY() flpf_gety(analogRead(Y_PIN), GLOBAL_Y_LOWPASS_FILTER)

#endif //EXPONENTIALAVERAGINGFILTER_H
