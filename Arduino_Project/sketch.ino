#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>

#define LCD_RS PB6
#define LCD_E  PB5
#define LCD_D4 PB4
#define LCD_D5 PH6
#define LCD_D6 PH5
#define LCD_D7 PH4
#define CS_LOW()  (PORTB &= ~(1 << PB0))
#define CS_HIGH() (PORTB |= (1 << PB0))

uint8_t row_pins[4] = {PC5, PC4, PC3, PC2};
uint8_t col_pins[4] = {PC1, PC0, PD7, PG2};

void lcd_enable() {
  PORTB |= (1 << LCD_E);
  _delay_us(1);
  PORTB &= ~(1 << LCD_E);
  _delay_us(100);
}

void lcd_send_nibble(uint8_t nibble) {
  (nibble & 0x01) ? (PORTB |= (1 << LCD_D4)) : (PORTB &= ~(1 << LCD_D4));
  (nibble & 0x02) ? (PORTH |= (1 << LCD_D5)) : (PORTH &= ~(1 << LCD_D5));
  (nibble & 0x04) ? (PORTH |= (1 << LCD_D6)) : (PORTH &= ~(1 << LCD_D6));
  (nibble & 0x08) ? (PORTH |= (1 << LCD_D7)) : (PORTH &= ~(1 << LCD_D7));
  lcd_enable();
}

void lcd_command(uint8_t cmd) {
  PORTB &= ~(1 << LCD_RS);
  lcd_send_nibble(cmd >> 4);
  lcd_send_nibble(cmd & 0x0F);
  _delay_ms(2);
}

void lcd_data(uint8_t data) {
  PORTB |= (1 << LCD_RS);
  lcd_send_nibble(data >> 4);
  lcd_send_nibble(data & 0x0F);
  _delay_ms(2);
}

void lcd_init() {
  DDRB |= (1 << LCD_RS) | (1 << LCD_E) | (1 << LCD_D4);
  DDRH |= (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7);
  _delay_ms(20);
  lcd_command(0x33); lcd_command(0x32);
  lcd_command(0x28); lcd_command(0x0C);
  lcd_command(0x06); lcd_command(0x01);
  _delay_ms(2);
}

void lcd_clear() { lcd_command(0x01); _delay_ms(2); }
void lcd_print(const char* str) { while (*str) lcd_data(*str++); }

// --- KEYPAD ---
void keypad_init() {
  for (int i = 0; i < 4; i++) { DDRC |= (1 << row_pins[i]); PORTC |= (1 << row_pins[i]); }
  DDRC &= ~((1 << PC1) | (1 << PC0)); PORTC |= (1 << PC1) | (1 << PC0);
  DDRD &= ~(1 << PD7); PORTD |= (1 << PD7);
  DDRG &= ~(1 << PG2); PORTG |= (1 << PG2);
}

char keypad_read() {
  const char keys[4][4] = {{'1','2','3','A'},{'4','5','6','B'},{'7','8','9','C'},{'*','0','#','D'}};
  for (int r = 0; r < 4; r++) {
    for (int i = 0; i < 4; i++) PORTC |= (1 << row_pins[i]);
    PORTC &= ~(1 << row_pins[r]);
    _delay_us(5);
    for (int c = 0; c < 4; c++) {
      uint8_t val = (c == 0) ? PINC & (1 << PC1) :
                    (c == 1) ? PINC & (1 << PC0) :
                    (c == 2) ? PIND & (1 << PD7) :
                               PING & (1 << PG2);
      if (!val) { _delay_ms(200); return keys[r][c]; }
    }
  }
  return 0;
}

void buzzer_init() { DDRB |= (1 << PB7); PORTB &= ~(1 << PB7); }
void buzzer_on()   { PORTB |= (1 << PB7); }
void buzzer_off()  { PORTB &= ~(1 << PB7); }
void buzzer_beep_ms(uint16_t d) { buzzer_on(); _delay_ms(d); buzzer_off(); }

void ledbar_init() { DDRA = 0xFF; DDRC |= (1 << PC6) | (1 << PC7); }
void ledbar_clear(){ PORTA = 0x00; PORTC &= ~((1 << PC6) | (1 << PC7)); }
void ledbar_set_level(uint8_t l){ ledbar_clear(); for (uint8_t i = 0; i < l; i++) i<8 ? (PORTA |= (1 << i)) : (PORTC |= (1 << PC7)); }

void urgency_leds_init() { DDRE |= (1 << PE3) | (1 << PE5); DDRG |= (1 << PG5); }
void urgency_leds_clear(){ PORTE &= ~((1 << PE3) | (1 << PE5)); PORTG &= ~(1 << PG5); }
void urgency_leds_set(uint8_t c){ urgency_leds_clear(); if (c>=1) PORTE|=(1<<PE3); if(c>=2) PORTE|=(1<<PE5); if(c==3) PORTG|=(1<<PG5); }

void spi_init() { DDRB |= (1 << PB0)|(1 << PB1)|(1 << PB2); DDRB &= ~(1 << PB3); PORTB |= (1 << PB3); SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0); }
uint8_t spi_transfer(uint8_t d){ SPDR = d; while (!(SPSR & (1 << SPIF))); return SPDR; }

uint8_t sd_send_command(uint8_t cmd, uint32_t arg, uint8_t crc){
  CS_LOW(); spi_transfer(0x40 | cmd);
  spi_transfer(arg >> 24); spi_transfer(arg >> 16); spi_transfer(arg >> 8); spi_transfer(arg); spi_transfer(crc);
  for (uint8_t i=0; i<10; i++) { uint8_t r = spi_transfer(0xFF); if (r != 0xFF) return r; } return 0xFF;
}

uint8_t sd_init() {
  CS_HIGH(); for (uint8_t i=0; i<10; i++) spi_transfer(0xFF);
  if (sd_send_command(0, 0, 0x95) != 0x01) return 0;
  if (sd_send_command(1, 0, 0xFF) != 0x00) return 0;
  CS_HIGH(); spi_transfer(0xFF); return 1;
}

uint8_t sd_write_block(const char* txt) {
  CS_LOW();
  if (sd_send_command(24, 0x00000100, 0xFF) != 0x00) { CS_HIGH(); return 0; }
  spi_transfer(0xFE);
  for (uint16_t i=0; i<512; i++) spi_transfer(i<strlen(txt) ? txt[i] : 0x00);
  spi_transfer(0xFF); spi_transfer(0xFF);
  if ((spi_transfer(0xFF) & 0x1F) != 0x05) { CS_HIGH(); return 0; }
  while (!spi_transfer(0xFF));
  CS_HIGH(); spi_transfer(0xFF); return 1;
}

char* get_log_message(char r, char u){
  static char msg[64];
  const char* rec = (r=='1')?"Nurse":(r=='2')?"Doctor":"Family";
  const char* urg = (u=='4')?"Low":(u=='5')?"Med":"High";
  sprintf(msg, "To:%s, Level:%s", rec, urg);
  return msg;
}

int main(void) {
  lcd_init(); keypad_init(); ledbar_init(); urgency_leds_init(); buzzer_init(); spi_init();
  uint8_t sd_ready = sd_init();

  lcd_print("Select Recipient");
  char recipient = 0;
  while (!recipient) {
    char k = keypad_read();
    if (k=='1'||k=='2'||k=='3') {
      recipient = k; lcd_clear();
      if (k=='1') lcd_print("To: Nurse");
      else if (k=='2') lcd_print("To: Doctor");
      else lcd_print("To: Family");
      lcd_command(0xC0); lcd_print("Level: ");
    }
  }

  while (1) {
    char k = keypad_read();
    if (k=='4'||k=='5'||k=='6') {
      lcd_clear();
      if (recipient=='1') lcd_print("To: Nurse");
      else if (recipient=='2') lcd_print("To: Doctor");
      else lcd_print("To: Family");

      lcd_command(0xC0); lcd_print("Level: ");
      if (k=='4') { lcd_print("Low"); ledbar_set_level(3); urgency_leds_set(1); }
      else if (k=='5') { lcd_print("Med"); ledbar_set_level(6); urgency_leds_set(2); if(recipient=='1') buzzer_beep_ms(1000); }
      else if (k=='6') { lcd_print("High"); ledbar_set_level(9); urgency_leds_set(3); if(recipient=='1') buzzer_beep_ms(2000); }

      char* log = get_log_message(recipient, k);
      lcd_clear(); lcd_print("Sent to ");
      if (recipient=='1') lcd_print("Nurse");
      else if (recipient=='2') lcd_print("Doctor");
      else lcd_print("Family");
      lcd_command(0xC0);
      if (k=='4') lcd_print("Level: Low");
      else if (k=='5') lcd_print("Level: Med");
      else if (k=='6') lcd_print("Level: High");

      if (sd_ready) sd_write_block(log);
      break;
    }
  }

  while(1);
}