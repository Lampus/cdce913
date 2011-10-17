/*
 * Driver for Texas Instruments
 * Programmable 1-PLL VCXO Clock Synthesizer CDCE913
 *
 * Copyright (C) 2011 Thesys-Intechna
 *
 * Licensed under the GPL-2 or later.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include "cdce913.h"

#define DRV_NAME	"cdce913"

struct pll_conf {
	unsigned vco_range: 2;
	unsigned p: 3;
	unsigned q: 6;
	unsigned r: 9;
	unsigned n: 12;
};

struct cdce913_pll {
	struct i2c_client *client;
	struct mutex lock;
	u16 pdiv[3];
	u8 y1;
	u8 y2y3;
	u8 fs1;
	struct pll_conf pll[2];
};

static int cdce913_read(struct i2c_client *client, u8 reg)
{
	int ret = i2c_smbus_read_byte_data(client, reg);

	if (ret < 0)
		dev_err(&client->dev, "Read Error\n");

	return ret;
}

static int cdce913_write(struct i2c_client *client, u8 reg, u8 value)
{
	int ret = i2c_smbus_write_byte_data(client, reg, value);

	if (ret < 0)
		dev_err(&client->dev, "Write Error\n");

	return ret;
}

static ssize_t cdce913_show_pdiv(struct device *dev,
				struct device_attribute *attr,
                char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client);

	return scnprintf(buf, PAGE_SIZE, "0x%04X,0x%02X,0x%02X\n", dev_data->pdiv[0], dev_data->pdiv[1], dev_data->pdiv[2]);
}

static ssize_t cdce913_store_pdiv(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t count)
{
	unsigned long tmp;
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client); 
	u8 pdiv_num;
	u16 pdiv_value;
	if (strict_strtoul(buf, 16, &tmp) < 0)
		return -EINVAL;
	// ! (pdiv_num > 3)
	// ! (pdiv_value > 1023)
	if(tmp&0xCC00UL)
		return -EINVAL;
	pdiv_num=(u8)((tmp&0xF000UL)>>12);
	pdiv_value=(u16)(tmp&0x03FFUL);
	if(pdiv_num==0)
		return -EINVAL;
	if((pdiv_num>1)&&(pdiv_value>127))
		return -EINVAL;
	mutex_lock(&dev_data->lock);
	dev_data->pdiv[pdiv_num-1] = pdiv_value;
	// FIXME!!! -->
	switch(pdiv_num) {
		case 1:
		cdce913_write(client, CDCE913_REG(PDIV1_70), (u8)pdiv_value);
		cdce913_write(client, CDCE913_REG(PDIV1_98), (u8)(pdiv_value>>8));
		break;
		case 2:
		cdce913_write(client, CDCE913_REG(PDIV2), (u8)pdiv_value);
		break;
		case 3:
		cdce913_write(client, CDCE913_REG(PDIV3), (u8)pdiv_value);
		break;
		default:
		;
	};
	mutex_unlock(&dev_data->lock);
	return count;
}

static ssize_t cdce913_show_y1(struct device *dev,
				struct device_attribute *attr,
                char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client);

	return scnprintf(buf, PAGE_SIZE, "0x%04X\n", dev_data->y1);
}

static ssize_t cdce913_store_y1(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t count)
{
	unsigned long tmp;
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client); 
	
	if (strict_strtoul(buf, 16, &tmp) < 0)
		return -EINVAL;
	if(tmp&0xFFFFFFF0UL)
		return -EINVAL;
	
	mutex_lock(&dev_data->lock);
	dev_data->y1 = (u8)tmp;
	cdce913_write(client, CDCE913_REG(Y1_X), (u8)tmp);
	mutex_unlock(&dev_data->lock);
	return count;
}

static ssize_t cdce913_show_y2y3(struct device *dev,
				struct device_attribute *attr,
                char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client);

	return scnprintf(buf, PAGE_SIZE, "0x%04X\n", dev_data->y2y3);
}

static ssize_t cdce913_store_y2y3(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t count)
{
	unsigned long tmp;
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client); 
	
	if (strict_strtoul(buf, 16, &tmp) < 0)
		return -EINVAL;
	if(tmp&0xFFFFFFF0UL)
		return -EINVAL;
	
	mutex_lock(&dev_data->lock);
	dev_data->y2y3 = (u8)tmp;
	cdce913_write(client, CDCE913_REG(Y2Y3_X), (u8)tmp);
	mutex_unlock(&dev_data->lock);
	return count;
}

static ssize_t cdce913_show_fs1(struct device *dev,
				struct device_attribute *attr,
                char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client);

	return scnprintf(buf, PAGE_SIZE, "0x%04X\n", dev_data->fs1);
}

static ssize_t cdce913_store_fs1(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t count)
{
	unsigned long tmp;
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client); 
	
	if (strict_strtoul(buf, 16, &tmp) < 0)
		return -EINVAL;
	if(tmp&0xFFFFFFF0UL)
		return -EINVAL;
	
	mutex_lock(&dev_data->lock);
	dev_data->fs1 = (u8)tmp;
	cdce913_write(client, CDCE913_REG(FS1_X), (u8)tmp);
	mutex_unlock(&dev_data->lock);
	return count;
}

static DEVICE_ATTR(pdiv, S_IWUSR|S_IRUSR, cdce913_show_pdiv, cdce913_store_pdiv);
static DEVICE_ATTR(y1, S_IWUSR|S_IRUSR, cdce913_show_y1, cdce913_store_y1);
static DEVICE_ATTR(y2y3, S_IWUSR|S_IRUSR, cdce913_show_y2y3, cdce913_store_y2y3);
static DEVICE_ATTR(fs1, S_IWUSR|S_IRUSR, cdce913_show_fs1, cdce913_store_fs1);
/*
static DEVICE_ATTR(pll1_0, S_IWUSR|S_IRUSR, cdce913_show_pll1_0, cdce913_store_pll1_0);
static DEVICE_ATTR(pll1_1, S_IWUSR|S_IRUSR, cdce913_show_pll1_1, cdce913_store_pll1_1);
static DEVICE_ATTR(ssc1, S_IWUSR|S_IRUSR, cdce913_show_ssc1, cdce913_store_ssc1);
*/
static struct attribute *cdce913_attributes[] = {
	&dev_attr_pdiv.attr,
	&dev_attr_y1.attr,
	&dev_attr_y2y3.attr,
	&dev_attr_fs1.attr,
	NULL
};

static const struct attribute_group cdce913_attr_group = {
	.attrs = cdce913_attributes,
};

static int __devinit cdce913_probe(struct i2c_client *client,
					const struct i2c_device_id *id)
{
	int ret = 0;
	struct cdce913_pll *dev;
	
	if (!i2c_check_functionality(client->adapter,
					I2C_FUNC_SMBUS_BYTE_DATA)) {
		dev_err(&client->dev, "SMBUS Byte Data not Supported\n");
		return -EIO;
	}
	
	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (dev == NULL) {
		dev_err(&client->dev, "failed to alloc memory\n");
		return -ENOMEM;
	}
	mutex_init(&dev->lock);
	
	i2c_set_clientdata(client, dev);
	
	return sysfs_create_group(&client->dev.kobj, &cdce913_attr_group);
	
err:
	kfree(dev);
	return ret;
}

static int __devexit cdce913_remove(struct i2c_client *client)
{
	struct cdce913_pll *dev = i2c_get_clientdata(client);
	kfree(dev);
	sysfs_remove_group(&client->dev.kobj, &cdce913_attr_group);
	return 0;
}

static const struct i2c_device_id cdce913_id[] = {
	{DRV_NAME, 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, cdce913_id);

static struct i2c_driver cdce913_driver = {
	.driver = {
		   .name = DRV_NAME,
		   },
	.probe = cdce913_probe,
	.remove = __devexit_p(cdce913_remove),
	.id_table = cdce913_id,
};

static int __init cdce913_init(void)
{
	return i2c_add_driver(&cdce913_driver);
}

module_init(cdce913_init);

static void __exit cdce913_exit(void)
{
	i2c_del_driver(&cdce913_driver);
}

module_exit(cdce913_exit);

MODULE_AUTHOR("Lapin Roman <lampus.lapin@gmail.com>");
MODULE_DESCRIPTION("TI CDCE913 Clk Driver");
MODULE_LICENSE("GPL");
