#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

static int read_reg(int fd, int reg_num, char* val)
{
	int res;
	if (lseek(fd, reg_num, SEEK_SET) != reg_num) {
		printf("could not seek in ksz8795 phy\n");
		return -1;
	}
	res = read(fd, val, sizeof(*val));
	if (res!=1) {
		printf("could not read from ksz8795 phy, reg_num=0x%x, errno=%d, res=%d\n",
			reg_num, errno, res);
		return -1;
	}
	return 0;
}

static int write_reg(int fd, int reg_num, char val)
{
	if (lseek(fd, reg_num, SEEK_SET) != reg_num) {
		printf("could not seek in ksz8795 phy\n");
		return -1;
	}
	if (write(fd, &val, sizeof(val)) != 1) {
		printf("could not write to ksz8795 phy, reg_num=0x%x, errno=%d\n",
			reg_num, errno);
		return -1;
	}
	return 0;
}

static int read_global_reg(int fd, int reg_num, char *val)
{
	if (write_reg(fd, 0x6e, 0x30))
		return -1;
	if (write_reg(fd, 0x6f, reg_num))
		return -1;
	if (read_reg(fd, 0xa0, val))
		return -1;
	return 0;
}

static int write_global_reg(int fd, int reg_num, char val)
{
	if (write_reg(fd, 0x6e, 0x20))
		return -1;
	if (write_reg(fd, 0x6f, reg_num))
		return -1;
	if (write_reg(fd, 0xa0, val))
		return -1;
	return 0;
}

int main(int argc, char **argv)
{
	int fd;
	char reg;
	fd = open("/sys/bus/spi/drivers/spi-ks8995/spi4.0/registers", O_RDWR);
	if (fd < 0)
		fd = open("/sys/bus/spi/drivers/spi-ks8795/spi32765.0/registers", O_RDWR);
	if (fd < 0) {
		printf("could not find ksz8795 phy - is it there?\n");
		return -1;
	}
	if (read_global_reg(fd, 0x35, &reg)) {
		printf("could not read global register 0x35\n");
		return -1;
	}
	printf("global register 0x35 = 0x%x\n", reg);
	reg &= ~0b1111;
	if (write_global_reg(fd, 0x35, reg)) {
		printf("could not write global register 0x35\n");
		return -1;
	}
	printf("global register 0x35 <= 0x%x\n", reg);
	printf("ksz8795 erratum 'Link drops with some EEE link partners' applied\n");
	return 0;
}
