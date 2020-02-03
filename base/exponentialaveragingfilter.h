#ifndef EXPONENTIALAVERAGINGFILTER_H
#define EXPONENTIALAVERAGINGFILTER_H

#define DEAD_BAND_VARIANCE 10

int DEAD_BAND_MIDDLE_X = 512;
int DEAD_BAND_MIDDLE_Y = 512;

struct float_lowpass_filter {
  float alpha;
  float y_prev;
  unsigned int pin;
};

struct float_lowpass_filter GLOBAL_X_LOWPASS_FILTER;
struct float_lowpass_filter GLOBAL_Y_LOWPASS_FILTER;

#define CREATE_EXPONENTIAL_FILTER(NAME) struct float_lowpass_filter NAME

void init_exponential_average_filter(struct float_lowpass_filter* filter, float alpha, int pin) {
  filter->alpha = alpha;
  filter->y_prev = analogRead(pin);
  filter->pin = pin;
}

float run_exponential_average_filter(struct float_lowpass_filter* filter) {
  int val = analogRead(filter->pin);
  if((val > (DEAD_BAND_MIDDLE_X - DEAD_BAND_VARIANCE)) && (val < (DEAD_BAND_MIDDLE_X + DEAD_BAND_VARIANCE)))
    val = DEAD_BAND_MIDDLE_X;
  filter->y_prev = filter->alpha*val + (1.0 - filter->alpha)*filter->y_prev;
  return filter->y_prev;
}


#endif //EXPONENTIALAVERAGINGFILTER_H
