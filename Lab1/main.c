#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

const uint8_t segments[] ={0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
uint8_t minus = 0x40;
const uint8_t testing_segments[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7};

int16_t angle = 12345;

uint8_t get_digit(int16_t d, uint8_t m) {
  uint8_t i = 5, a;
  while(i) {
    a = d%10;
    if (i-- == m) break;
    d /=10;
  }
  return a;
}

ISR(INT1_vect) {
  if ((PIND & (1 << 1)) != 0) {
    EICRA = (1 << ISC11)|( 1 << ISC21);
    if ((PIND & (1 << 0)) != 0) angle--;
    else angle++;
  } 
  else {
      EICRA = (1 << ISC11) | (1 << ISC10)| ( 1 << ISC21);
      if ((PIND & (1 << 0)) != 0) angle++;
      else angle--;
  }
}

ISR(INT2_vect) {
  angle = 0;
}

void print_number(int8_t number, uint8_t place) {
  PORTA = 1 << place;
  PORTC = number;
  PORTA &=~ (1 << place);
}

void clear() {
  for (uint8_t i = 0; i <= 5; i++) {
    print_number(0, i);
  }
}


void print_full_number(int16_t number) {
  clear();
  if (number < 0) {
      print_number(minus, 1);
      number = 0-number;
  } else {
    print_number(segments[get_digit(number, 1)], 1);
  }
  for (uint8_t i = 2; i <= 5; i++) {
      print_number(segments[get_digit(number, i)], i);
  }
}

int main()
{
  EICRA = (1 << ISC11)|( 1 << ISC21);
  EIMSK = (1 << INT1) | (1 << INT2);
  DDRA = 0xFF;
  DDRC = 0xFF;
  sei();
  while (1)  {
    print_full_number(angle);
    _delay_ms(30);
  }
  return 0;
}
