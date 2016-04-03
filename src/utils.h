
void mode_PORTA_INPUT(unsigned int pin);
void mode_PORTA_OUTPUT(unsigned int pin);
void write_PORTA_HIGH(unsigned int pin);
void write_PORTA_LOW(unsigned int pin);
int read_PORTA(unsigned int pin);

void init_ADC();
uint16_t read_ADC(uint8_t ch);