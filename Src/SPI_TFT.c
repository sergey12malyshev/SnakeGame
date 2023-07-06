#include <stdlib.h> // abs()

#include "stm32f1xx_hal.h"
#include "SPI_TFT.h"
#include "font.h"
#include "math.h"
#include "string.h"

// массив для DMA и отправки пакетов по SPI (чем больше шрифты по размеру, тем больше массив нужно)
uint8_t buf[4880];

extern SPI_HandleTypeDef hspi1; // используемый модуль SPI

// HAL'овская инициализация выводов DC и CS
void GPIO_init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct1;
	GPIO_InitTypeDef GPIO_InitStruct2;

	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	HAL_GPIO_WritePin(TFT_DC_PORT, TFT_DC_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_RESET);

	GPIO_InitStruct1.Pin = TFT_DC_PIN;
	GPIO_InitStruct1.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct1.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(TFT_DC_PORT, &GPIO_InitStruct1);

	GPIO_InitStruct2.Pin = TFT_CS_PIN;
	GPIO_InitStruct2.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct2.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(TFT_CS_PORT, &GPIO_InitStruct2);
}

// шлем команду
// по SPI через прерывания
void LCD_SendCommand(uint8_t com)
{
	HAL_GPIO_WritePin(TFT_DC_PORT, TFT_DC_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit_IT(&hspi1, &com, 1);
	HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_SET);
}

// шлем данные
// по SPI через прерывания
void LCD_SendData(uint8_t data)
{
	HAL_GPIO_WritePin(TFT_DC_PORT, TFT_DC_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit_IT(&hspi1, &data, 1);
	HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_SET);
}

// инициализация дисплея ILI9341
void LCD_Init(void)
{
	// настраиваем ноги
	GPIO_init();

	// сброс дисплея
	// TFT_RST_SET;   /* Не подключен, надо бы подключить */
	LCD_SendCommand(LCD_SWRESET);
	HAL_Delay(100);

	// настраиваем дисплей
	LCD_SendCommand(LCD_POWERA);
	LCD_SendData(0x39);
	LCD_SendData(0x2C);
	LCD_SendData(0x00);
	LCD_SendData(0x34);
	LCD_SendData(0x02);
	LCD_SendCommand(LCD_POWERB);
	LCD_SendData(0x00);
	LCD_SendData(0xC1);
	LCD_SendData(0x30);
	LCD_SendCommand(LCD_DTCA);
	LCD_SendData(0x85);
	LCD_SendData(0x00);
	LCD_SendData(0x78);
	LCD_SendCommand(LCD_DTCB);
	LCD_SendData(0x00);
	LCD_SendData(0x00);
	LCD_SendCommand(LCD_POWER_SEQ);
	LCD_SendData(0x64);
	LCD_SendData(0x03);
	LCD_SendData(0x12);
	LCD_SendData(0x81);
	LCD_SendCommand(LCD_PRC);
	LCD_SendData(0x20);
	LCD_SendCommand(LCD_POWER1);
	LCD_SendData(0x23);
	LCD_SendCommand(LCD_POWER2);
	LCD_SendData(0x10);
	LCD_SendCommand(LCD_VCOM1);
	LCD_SendData(0x3E);
	LCD_SendData(0x28);
	LCD_SendCommand(LCD_VCOM2);
	LCD_SendData(0x86);
	LCD_SendCommand(LCD_MAC);
	LCD_SendData(0x48);
	LCD_SendCommand(LCD_PIXEL_FORMAT);
	LCD_SendData(0x55);
	LCD_SendCommand(LCD_FRMCTR1);
	LCD_SendData(0x00);
	LCD_SendData(0x18);
	LCD_SendCommand(LCD_DFC);
	LCD_SendData(0x08);
	LCD_SendData(0x82);
	LCD_SendData(0x27);
	LCD_SendCommand(LCD_3GAMMA_EN);
	LCD_SendData(0x00);
	LCD_SendCommand(LCD_COLUMN_ADDR);
	LCD_SendData(0x00);
	LCD_SendData(0x00);
	LCD_SendData(0x00);
	LCD_SendData(0xEF);
	LCD_SendCommand(LCD_PAGE_ADDR);
	LCD_SendData(0x00);
	LCD_SendData(0x00);
	LCD_SendData(0x01);
	LCD_SendData(0x3F);
	LCD_SendCommand(LCD_GAMMA);
	LCD_SendData(0x01);
	LCD_SendCommand(LCD_PGAMMA);
	LCD_SendData(0x0F);
	LCD_SendData(0x31);
	LCD_SendData(0x2B);
	LCD_SendData(0x0C);
	LCD_SendData(0x0E);
	LCD_SendData(0x08);
	LCD_SendData(0x4E);
	LCD_SendData(0xF1);
	LCD_SendData(0x37);
	LCD_SendData(0x07);
	LCD_SendData(0x10);
	LCD_SendData(0x03);
	LCD_SendData(0x0E);
	LCD_SendData(0x09);
	LCD_SendData(0x00);
	LCD_SendCommand(LCD_NGAMMA);
	LCD_SendData(0x00);
	LCD_SendData(0x0E);
	LCD_SendData(0x14);
	LCD_SendData(0x03);
	LCD_SendData(0x11);
	LCD_SendData(0x07);
	LCD_SendData(0x31);
	LCD_SendData(0xC1);
	LCD_SendData(0x48);
	LCD_SendData(0x08);
	LCD_SendData(0x0F);
	LCD_SendData(0x0C);
	LCD_SendData(0x31);
	LCD_SendData(0x36);
	LCD_SendData(0x0F);
	LCD_SendCommand(LCD_SLEEP_OUT);

	HAL_Delay(100);
	LCD_SendCommand(LCD_DISPLAY_ON);
	LCD_SendCommand(LCD_GRAM);
}

// устанавливаем курсор в заданную точку х1 у1 и выделяем область для заполнения пикселей х1у1-х2у2
void LCD_SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	LCD_SendCommand(LCD_COLUMN_ADDR);
	LCD_SendData(x1 >> 8);
	LCD_SendData(x1 & 0xFF);
	LCD_SendData(x2 >> 8);
	LCD_SendData(x2 & 0xFF);

	LCD_SendCommand(LCD_PAGE_ADDR);
	LCD_SendData(y1 >> 8);
	LCD_SendData(y1 & 0xFF);
	LCD_SendData(y2 >> 8);
	LCD_SendData(y2 & 0xFF);
}

// закраска всего экрана цветом
void LCD_Fill(uint16_t color)
{
	unsigned int n, i, j, t;
	i = color >> 8;
	j = color & 0xFF;
	LCD_SetCursorPosition(0, 0, width - 1, height - 1);

	for (n = 0; n < 640; n += 2)
	{
		buf[n] = i;
		buf[n + 1] = j;
	}

	LCD_SendCommand(LCD_GRAM);

	HAL_GPIO_WritePin(TFT_DC_PORT, TFT_DC_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_RESET);

	for (n = 0; n < 240; n++)
	{

		HAL_SPI_Transmit_DMA(&hspi1, buf, 640);

		for (t = 0; t < 2900; t++)
		{
			__NOP();
		}
	}

	HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_SET);
}

// закраска цветом выделенной области относительно точки х1у1 на длину и ширину w и h выбранным цветом
void LCD_fillRect(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t color)
{
	unsigned int n, i, j, t;
	i = color >> 8;
	j = color & 0xFF;
	uint16_t num = ((w * h * 2) / 640) + 1;
	uint16_t nnum = w * h * 2;
	LCD_SetCursorPosition(x1, y1, (uint16_t)(x1 + w - 1), (uint16_t)(y1 + h - 1));

	for (n = 0; n < 640; n += 2)
	{
		buf[n] = i;
		buf[n + 1] = j;
	}
	LCD_SendCommand(LCD_GRAM);

	HAL_GPIO_WritePin(TFT_DC_PORT, TFT_DC_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_RESET);

	if (nnum > 640)
	{

		for (n = 0; n < num; n++)
		{

			HAL_SPI_Transmit_DMA(&hspi1, buf, 640);

			for (t = 0; t < 2900; t++)
			{
				__NOP();
			}
		}
	}

	else
	{
		HAL_SPI_Transmit_DMA(&hspi1, buf, nnum);
		for (t = 0; t < nnum * 3 + nnum / 14; t++)
		{
			__NOP();
		}
	}

	HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_SET);
}

// закрасить пиксель выбранным цветом по координатам х-у
void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
	LCD_SetCursorPosition(x, y, x, y);
	LCD_SendCommand(LCD_GRAM);
	LCD_SendData(color >> 8);
	LCD_SendData(color & 0xFF);
}

// задать ориентацию экрана (вериткально, горизонтально, зеркально)
void LCD_setOrientation(uint8_t o)
{
	if (o == ORIENTATION_LANDSCAPE || o == ORIENTATION_LANDSCAPE_MIRROR)
	{
		height = LCD_PIXEL_WIDTH;
		width = LCD_PIXEL_HEIGHT;
	}
	else
	{
		height = LCD_PIXEL_HEIGHT;
		width = LCD_PIXEL_WIDTH;
	}
	LCD_SendCommand(LCD_MAC);
	LCD_SendData(o);
}

// нарисовать линию (прямую) заданного цвета между точками х0у0 и х1у1
void line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
	int x = x1 - x0;
	int y = y1 - y0;
	int dx = abs(x), sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2; // error value e_xy
	for (;;)
	{
		LCD_DrawPixel(x0, y0, color);
		e2 = 2 * err;
		if (e2 >= dy) // e_xy+e_x > 0
		{
			if (x0 == x1)
				break;
			err += dy;
			x0 += sx;
		}
		if (e2 <= dx) // e_xy+e_y < 0
		{
			if (y0 == y1)
				break;
			err += dx;
			y0 += sy;
		}
	}
}

// нарисовать линию заданного цвета и толщины между точкам х0у0 - х1у1
void H_line(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int size, unsigned int color)
{
	int x = x1 - x0;
	int y = y1 - y0;
	int dx = abs(x), sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2; // error value e_xy
	for (;;)
	{
		LCD_fillRect(x0, y0, size, size, color);
		e2 = 2 * err;
		if (e2 >= dy) // e_xy+e_x > 0
		{
			if (x0 == x1)
				break;
			err += dy;
			x0 += sx;
		}
		if (e2 <= dx) // e_xy+e_y < 0
		{
			if (y0 == y1)
				break;
			err += dx;
			y0 += sy;
		}
	}
}

// нарисовать линию заданного цвета, толщины, длины (радиуса) (r) и повернутую на заданный угол относительно начальной точки х-у
void line_angle(uint16_t x, uint16_t y, uint16_t r, int Angle, uint16_t size, uint16_t color)
{

	int px, py;
	Angle -= 90;

	px = x + cos((Angle * 3.14) / 180) * r;
	py = y + sin((Angle * 3.14) / 180) * r;

	H_line(x, y, px, py, size, color);
}

// нарисовать круг с центром в х0у0 заданного радиуса и цвета
void circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
	int x = -r, y = 0, err = 2 - 2 * r, e2;
	do
	{
		LCD_DrawPixel(x0 - x, y0 + y, color);
		LCD_DrawPixel(x0 + x, y0 + y, color);
		LCD_DrawPixel(x0 + x, y0 - y, color);
		LCD_DrawPixel(x0 - x, y0 - y, color);
		e2 = err;
		if (e2 <= y)
		{
			err += ++y * 2 + 1;
			if (-x == y && e2 <= x)
				e2 = 0;
		}
		if (e2 > x)
			err += ++x * 2 + 1;
	} while (x <= 0);
}

// нарисовать закрашенный круг с центром в х0у0 заданного радиуса и цвета
void fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
	int x = -r, y = 0, err = 2 - 2 * r, e2;
	do
	{
		LCD_fillRect(x0 - x, y0 - y, 1, 2 * y, color);
		LCD_fillRect(x0 + x, y0 - y, 1, 2 * y, color);

		e2 = err;
		if (e2 <= y)
		{
			err += ++y * 2 + 1;
			if (-x == y && e2 <= x)
				e2 = 0;
		}
		if (e2 > x)
			err += ++x * 2 + 1;
	} while (x <= 0);
}

// нарисовать дугу с центром в точке х-у, заданного радиуса, от начального угла до конечного угла, заданной толщины дуги и толщины линии отрисовки, выбранного цвета
void arc(int x, int y, int r, int startAngle, int endAngle, int thickness, uint16_t size, unsigned int color)
{
	int rDelta = -(thickness / 2);
	int px, py, cx, cy;

	startAngle -= 90;
	endAngle -= 90;

	if (startAngle != endAngle)
	{
		for (int i = 0; i < thickness; i++)
		{
			px = x + cos((startAngle * 3.14) / 180) * (r + rDelta + i);
			py = y + sin((startAngle * 3.14) / 180) * (r + rDelta + i);
			for (int d = startAngle + 1; d < endAngle + 1; d++)
			{
				cx = x + cos((d * 3.14) / 180) * (r + rDelta + i);
				cy = y + sin((d * 3.14) / 180) * (r + rDelta + i);
				H_line(px, py, cx, cy, size, color);
				px = cx;
				py = cy;
			}
		}
	}
	else
	{
		px = x + cos((startAngle * 3.14) / 180) * (r + rDelta);
		py = y + sin((startAngle * 3.14) / 180) * (r + rDelta);
		cx = x + cos((startAngle * 3.14) / 180) * (r - rDelta);
		cy = y + sin((startAngle * 3.14) / 180) * (r - rDelta);
		H_line(px, py, cx, cy, size, color);
	}
}

// задаем фиксированные области сверху и снизу, которые не участвуют в скроллинге
// скроллинг только вертикальный, то есть вдоль 320 пикселей
void LCD_setVerticalScrolling(uint16_t startY, uint16_t endY)
{
	LCD_SendCommand(LCD_VSCRDEF);

	buf[0] = startY >> 8;
	buf[1] = startY & 0xFF;
	buf[2] = (LCD_PIXEL_HEIGHT - startY - endY) >> 8;
	buf[3] = (LCD_PIXEL_HEIGHT - startY - endY) & 0xFF;
	buf[4] = endY >> 8;
	buf[5] = endY & 0xFF;

	LCD_SendData(buf[0]);
	LCD_SendData(buf[1]);
	LCD_SendData(buf[2]);
	LCD_SendData(buf[3]);
	LCD_SendData(buf[4]);
	LCD_SendData(buf[5]);
}

// задаем смещение для скроллинга
// на эту величину сместится область скроллинга, значение абсолютное, НЕ относительное
void LCD_scroll(uint16_t v)
{
	LCD_SendCommand(LCD_VSCRSADD);

	buf[0] = v >> 8;
	buf[1] = v & 0xFF;
	LCD_SendData(buf[0]);
	LCD_SendData(buf[1]);
}

// отрисовка символа простого шрифта в заданной точке х0у0, заданного размера и цветов шрифта и фона (алгоритм медленный, не используется DMA с буфером на символ)
void simple_font_OUT(uint8_t ascii, uint16_t x0, uint16_t y0, uint16_t size, uint16_t fgcolor, uint16_t bgcolor)
{
	if ((ascii >= 32) && (ascii <= 127)) // Characters below 32 are not printable, there are 127 of them
	{
		; // If printable do nothing
	}
	else
	{
		// If not printable write junk
		ascii = '?' - 32; // ascii = 63-32=31
	}
	// glcd_clr(x0, y0, size);	// delete before printing
	int i;
	for (i = 0; i < 8; i++)
	{

		// pgm_read_byte - Read a byte from the program space with a 16-bit (near) address.
		unsigned char temp = simpleFont[ascii - 32][i]; // because font.c starts at 0, not 32
		unsigned char f = 0;
		for (f = 0; f < 8; f++)
		{
			if ((temp >> f) & 0x01)

			{
				LCD_fillRect(x0 + i * size, y0 + f * size, size, size, fgcolor);
			}

			else
			{
				LCD_fillRect(x0 + i * size, y0 + f * size, size, size, bgcolor);
			}
		}
	}
}
//

// вывод строки символов простого шрифта с его параметрами
void simple_font_string_OUT(char *string, uint16_t x0, uint16_t y0, uint16_t size, uint16_t fgcolor, uint16_t bgcolor)
{
	while (*string)
	{
		simple_font_OUT(*string, x0, y0, size, fgcolor, bgcolor);
		string++;

		if (x0 < width)
			x0 += 6 * size;
	}
}
//

// вывод цифр (больших и красивых) в заданной точке х0у0, (size не используется), с выбранными цветами шрифта и фона шрифта
void NUM_OUT(uint8_t ascii, uint16_t x0, uint16_t y0, uint16_t size, uint16_t fgcolor, uint16_t bgcolor)
{
	unsigned int n, i, j, h, k, t, W, H, BS, Mass;
	i = fgcolor >> 8;
	j = fgcolor & 0xFF;
	h = bgcolor >> 8;
	k = bgcolor & 0xFF;
	int INDEX = 0;

	H = dSDigital_72ptDescriptors[ascii - 48][0]; // читаем количество бит на ширину и длину символа
	W = dSDigital_72ptDescriptors[ascii - 48][1];
	if (H % 8 != 0)
	{
		H = ((H / 8) + 1) * 8;
	}				// если делится без остатка, то все нормально, если нет, то округляем до значений байт, то есть до кратного 8 числа (в массиве все равно байты)
	BS = H * W * 2; // количество бит
	Mass = BS / 16; // количество байт (с учетом умножения BS на 2)

	LCD_SetCursorPosition(x0, y0, (uint16_t)(x0 + H - 1), (uint16_t)(y0 + W - 1));

	LCD_SendCommand(LCD_GRAM);
	HAL_GPIO_WritePin(TFT_DC_PORT, TFT_DC_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_RESET);

	for (n = 0; n < Mass; n++)

	{
		// unsigned char temp = dSDigital_72ptBitmaps[((ascii-48)*305)+n];	// because font.c starts at 0, not 32
		unsigned char temp = dSDigital_72ptBitmaps[dSDigital_72ptDescriptors[ascii - 48][2] + n]; // because font.c starts at 0, not 32
		unsigned char f = 0;
		for (f = 0; f < 8; f++)
		{

			if ((temp >> f) & 0x01)

			{
				buf[INDEX] = i;
				buf[INDEX += 1] = j;
				INDEX += 1;
			}

			else
			{
				buf[INDEX] = h;
				buf[INDEX += 1] = k;
				INDEX += 1;
			}
		}
	}
	HAL_SPI_Transmit_IT(&hspi1, buf, BS);
	for (t = 0; t < 12000; t++)
	{
		__NOP();
	}

	HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_SET);
}
//

// отрисовка символа шрифта (определенного размера - то что занесено в font.h) (size не используется) в заданной точке, с выбранными цветами шрифта и фона шрифта
// лучше использовать моноширинные шрифты (не совсем красиво, но и без переборов пропорциональных шрифтов - все символы примерно посередине независимо от толщины буков)
void FONT_OUT(uint8_t ascii, uint16_t x0, uint16_t y0, uint16_t size, uint16_t fgcolor, uint16_t bgcolor)
{

	unsigned int n, i, j, h, k, t, W, H, BS, Mass;
	i = fgcolor >> 8;
	j = fgcolor & 0xFF;
	h = bgcolor >> 8;
	k = bgcolor & 0xFF;
	int INDEX = 0;

	if (ascii == 32)
	{																										   // пробел, который не входит в массив шрифта
		LCD_fillRect(x0, y0, arialNarrow_18ptDescriptors[31][0], arialNarrow_18ptDescriptors[31][1], bgcolor); // пробел по размеру символа @
	}

	else
	{

		H = arialNarrow_18ptDescriptors[ascii - 33][0]; // читаем количество бит на ширину и длину символа
		W = arialNarrow_18ptDescriptors[ascii - 33][1];
		if (H % 8 != 0)
		{
			H = ((H / 8) + 1) * 8;
		}				// если делится без остатка, то все нормально, если нет, то округляем до значений байт, то есть до кратного 8 числа (в массиве все равно байты)
		BS = H * W * 2; // количество бит
		Mass = BS / 16; // количество байт (с учетом умножения BS на 2)

		LCD_SetCursorPosition(x0, y0, (uint16_t)(x0 + H - 1), (uint16_t)(y0 + W - 1));

		LCD_SendCommand(LCD_GRAM);
		HAL_GPIO_WritePin(TFT_DC_PORT, TFT_DC_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_RESET);

		for (n = 0; n < Mass; n++)

		{
			// переводим биты массива в байты со значением цвета в буфер SPI
			unsigned char temp = arialNarrow_18ptBitmaps[arialNarrow_18ptDescriptors[ascii - 33][2] + n]; // because font.c starts at 0, not 32
			unsigned char f = 0;
			for (f = 0; f < 8; f++)
			{

				if ((temp >> f) & 0x01)

				{
					buf[INDEX] = i;
					buf[INDEX += 1] = j;
					INDEX += 1;
				}

				else
				{
					buf[INDEX] = h;
					buf[INDEX += 1] = k;
					INDEX += 1;
				}
			}
		}
		HAL_SPI_Transmit_DMA(&hspi1, buf, BS);
		for (t = 0; t < 5000; t++)
		{
			__NOP();
		}

		HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_SET);
	}

	// return arialNarrow_18ptDescriptors[ascii-33][0]; // функция возвращает значение ширины символа
}
//

// вывод строки символов шрифта с его параметрами
void STRING_OUT(char *string, uint16_t x0, uint16_t y0, uint16_t size, uint16_t fgcolor, uint16_t bgcolor)
{

	while (*string)
	{
		FONT_OUT(*string, x0, y0, size, fgcolor, bgcolor);
		string++;

		if (x0 < width)
			x0 += arialNarrow_18ptDescriptors[31][0]; // тут нужно прибавлять ширину символа (т.к. моноширина шриффта, то все символы одинаковой ширины)
	}
}
//

// вывод числа с количеством символов (j) от 1 до 8 (упирается в размер буфера)
void STRING_NUM(unsigned int value, uint16_t j, uint16_t x0, uint16_t y0, uint16_t fgcolor, uint16_t bgcolor)
{
	unsigned char char_buffer[8] = "";
	unsigned char i = 0;

	while (value > 0)
	{
		char_buffer[i++] = value % 10;
		value /= 10;
	}
	for (; j > 0; j--)
	{
		NUM_OUT('0' + char_buffer[j - 1], x0, y0, 1, fgcolor, bgcolor);
		if (x0 < width)
			x0 += dSDigital_72ptDescriptors[0][0] + 16;
	}
}
//

// вывод числа с количеством символов (j) от 1 до 8 (упирается в размер буфера)
void STRING_NUM_L(unsigned int value, uint16_t j, uint16_t x0, uint16_t y0, uint16_t fgcolor, uint16_t bgcolor)
{
	unsigned char char_buffer[8] = "";
	unsigned char i = 0;

	while (value > 0)
	{
		char_buffer[i++] = value % 10;
		value /= 10;
	}
	for (; j > 0; j--)
	{
		FONT_OUT('0' + char_buffer[j - 1], x0, y0, 1, fgcolor, bgcolor);
		if (x0 < width)
			x0 += arialNarrow_18ptDescriptors[31][0];
	}
}
//

// отрисовка рисунка из файла (требует много памяти для хранения массива рисунка или внешний накопитель)
// редактировать универсально
void PIC(unsigned char const *img)
{
	uint32_t tt;

	LCD_SetCursorPosition(0, 0, 160 - 1, 107 - 1);
	LCD_SendCommand(LCD_GRAM);
	HAL_GPIO_WritePin(TFT_DC_PORT, TFT_DC_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_RESET);
	for (tt = 0; tt < 160 * 107; tt++)
	{
		uint8_t color = *img;
		img++;
		LCD_SendData(*img);
		LCD_SendData(color);
		img++;
	}
}
//
