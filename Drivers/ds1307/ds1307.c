/**
  ******************************************************************************
  * @file    ds1307.c
  * @author  erol-tesla
  * @brief   DS1307 RTC driver source file
  ******************************************************************************
  * @attention
  * This file provides the function implementations for DS1307 initialization
  * and time read operations.
  ******************************************************************************
  */


#include "ds1307.h"

ds1307_status_t ds1307_init(ds1307_dev_t *dev)
{
	if(!dev)
	{
		return DS1307_E_PARAM;
	}
    uint8_t buf[7];

    /* seconds (CH=0) */
    buf[0] = ds1307_bin_to_bcd(dev->time.seconds) & ~DS1307_SECONDS_CH_MASK;


    /* minutes */
    buf[1] = ds1307_bin_to_bcd(dev->time.minutes);

    /* hours */
    if (dev->time.time_format == DS1307_HOUR_24H)   /* 24h mode */
    {
        buf[2] = ds1307_bin_to_bcd(dev->time.hours) & 0x3F;
    }
    else  /* 12h mode */
    {
        buf[2]  = ds1307_bin_to_bcd(dev->time.hours & 0x1F);
        buf[2] |= DS1307_HOUR_12H;
        buf[2]  = (dev->time.meridiem == DS1307_PM) ? (buf[2] | DS1307_HOUR_AMPM) : (buf[2] & ~DS1307_HOUR_AMPM);
    }

    /* day, date, month, year */
    buf[3] = ds1307_bin_to_bcd(dev->time.day_of_week);
    buf[4] = ds1307_bin_to_bcd(dev->time.date);
    buf[5] = ds1307_bin_to_bcd(dev->time.month);
    buf[6] = ds1307_bin_to_bcd(dev->time.year);


    if(dev->i2c_write(dev->dev_addr, DS1307_REG_SECONDS, buf, 7U) != DS1307_OK)
    {
    	return DS1307_E_COMM;
    }

    uint8_t ctrl = ds1307_bin_to_bcd(0x00);
    if (dev->i2c_write(dev->dev_addr, DS1307_REG_CONTROL, &ctrl, 1U) != DS1307_OK)
    {
        return DS1307_E_COMM;
    }
    return DS1307_OK;
}


ds1307_status_t ds1307_get_time(ds1307_dev_t *dev)
{
    uint8_t buf[7];

    /* Burst read: seconds–year (0x00–0x06) */
    if(dev->i2c_read(dev->dev_addr, DS1307_REG_SECONDS, buf, 7U) != DS1307_OK)
    {
    	return DS1307_E_COMM;
    }

    /* Seconds: CH bit mask */
    dev->time.seconds = ds1307_bcd_to_bin(buf[0] & ~DS1307_SECONDS_CH_MASK);

    dev->time.minutes = ds1307_bcd_to_bin(buf[1] & 0x7F);
    /* Minutes */

    /* Hours */
    if (buf[2] & DS1307_HOUR_12H)   /* 12h mode */
    {
        dev->time.time_format = DS1307_HOUR_12H;
        dev->time.hours = ds1307_bcd_to_bin(buf[2] & 0x1F);  /* only bits0–4 */

        dev->time.meridiem = (buf[2] & DS1307_HOUR_AMPM) ? DS1307_PM : DS1307_AM;
    }
    else  /* 24h mode */
    {
        dev->time.time_format = DS1307_HOUR_24H;
        dev->time.hours = ds1307_bcd_to_bin(buf[2] & 0x3F);
        dev->time.meridiem = DS1307_AM;  /* kullanılmıyor ama default verelim */
    }

    /* Day, Date, Month, Year */
    dev->time.day_of_week = ds1307_bcd_to_bin(buf[3] & 0x07);
    dev->time.date        = ds1307_bcd_to_bin(buf[4] & 0x3F);
    dev->time.month       = ds1307_bcd_to_bin(buf[5] & 0x1F);
    dev->time.year        = ds1307_bcd_to_bin(buf[6]);

    return DS1307_OK;
}


const char* GetDayName(uint8_t day) {
    // 1-7 arası gün indeksi için
    const char* days[] = {"", "PAZARTESI", "SALI", "CARSAMBA", "PERSEMBE", "CUMA", "CUMARTESI", "PAZAR"};
    if (day < 1 || day > 7) return "";
    return days[day];
}

const char* GetMonthName(uint8_t month) {
    const char* months[] = {"", "OCAK", "SUBAT", "MART", "NISAN", "MAYIS", "HAZIRAN", 
                            "TEMMUZ", "AGUSTOS", "EYLUL", "EKIM", "KASIM", "ARALIK"};
    if (month < 1 || month > 12) return "";
    return months[month];
}


