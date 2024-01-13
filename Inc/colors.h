#ifndef __COLORS_H
#define __COLORS_H


inline uint16_t getOrange(void)
{
  const uint16_t orange_color = 0xFF00;
  return orange_color;
}

inline uint16_t getRed(void)
{
  const uint16_t red_color = COLOR(255, 0, 0);
  return red_color;
}

inline uint16_t getBlue(void)
{
  const uint16_t blue_color = COLOR(42, 165, 184);
  return blue_color;
}

inline uint16_t getGreen(void)
{
  const uint16_t green_color = COLOR(17, 255, 0);
  return green_color;
}

inline uint16_t getWhite(void)
{
  const uint16_t white_color = COLOR(255, 255, 255);
  return white_color;
}

inline uint16_t getBlack(void)
{
  const uint16_t black_color = 0x0000;
  return black_color;
}

inline uint16_t getYellow(void)
{
  const uint16_t yellow_color = COLOR(255, 255, 0);;
  return yellow_color;
}

inline uint16_t getPurple(void)
{
  const uint16_t purple_color = COLOR(204, 51, 255);
  return purple_color;
}

#endif /* __COLORS_H */