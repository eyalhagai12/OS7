#include "mylibc.h"

myFILE *myfopen(const char *restrict pathname, const char *restrict mode)
{
	myFILE *file = malloc(sizeof(myFILE));

	if (strcmp(mode, "r") == 0)
	{
		int file_d = myopen(pathname, 0);
		if (file_d == -1)
		{
			free(file);
			return NULL;
		}
		file->fd = file_d;
		file->read = 1;
		file->write = 0;
		return file;
	}
	else if (strcmp(mode, "r+") == 0)
	{
		int file_d = myopen(pathname, O_CREAT);
		if (file_d == -1)
		{
			free(file);
			return NULL;
		}
		file->fd = file_d;
		file->read = 1;
		file->write = 1;
		return file;
	}
	else if (strcmp(mode, "w") == 0)
	{
		int file_d = myopen(pathname, O_CREAT);
		if (file_d == -1)
		{
			free(file);
			return NULL;
		}
		file->fd = file_d;
		file->read = 0;
		file->write = 1;
		return file;
	}
	else if (strcmp(mode, "a") == 0)
	{
		int file_d = myopen(pathname, O_CREAT);
		if (file_d == -1)
		{
			free(file);
			return NULL;
		}
		file->fd = file_d;
		file->read = 1;
		file->write = 1;
		mylseek(file_d, 0, SEEK_END);
		return file;
	}

	free(file);
	return NULL;
}

int myfclose(myFILE *stream)
{
	myclose(stream->fd);
	free(stream);

	return 0;
}

size_t myfread(void *restrict ptr, size_t size, size_t nmemb, myFILE *restrict stream)
{
	if (stream->read == 1)
	{
		return myread(stream->fd, ptr, size * nmemb);
	}
	return 0;
}

size_t myfwrite(const void *restrict ptr, size_t size, size_t nmemb, myFILE *restrict stream)
{
	if (stream->write == 1)
	{
		return mywrite(stream->fd, ptr, size * nmemb);
	}
	return 0;
}

int myfseek(myFILE *stream, long offset, int whence)
{
	mylseek(stream->fd, offset, whence);
}

int myfscanf(myFILE *restrict stream, const char *restrict format, ...)
{
	va_list arguments;

	va_start(arguments, format);

	char tmp;

	for (size_t i = 0; i < strlen(format) - 1; ++i)
	{
		if (format[i] != '%')
		{
			if ((i > 0 && format[i - 1] != '%') || i == 0)
			{
				myread(stream->fd, &tmp, sizeof(char));
			}
		}
		if (format[i] == '%')
		{
			if (format[i + 1] == 'd')
			{

				int count = 0;
				char data = '0';
				char buff[128];
				int size = 9;

				while (((data <= '9' && data >= '0') || data == '-') && count < size)
				{
					myread(stream->fd, &data, sizeof(char));
					buff[count++] = data;
				}
				mylseek(stream->fd, -1, SEEK_CUR);
				buff[count] = '\0';
				*(int *)va_arg(arguments, int *) = atoi(buff);
			}
			else if (format[i + 1] == 'c')
			{
				char *num = va_arg(arguments, char *);
				char data;

				myread(stream->fd, &data, sizeof(char));
				*num = data;
			}
			else if (format[i + 1] == 'f')
			{
				int count = 0;
				char data = '0';
				char buff[128];
				int size = 9;

				while (((data <= '9' && data >= '0') || data == '-' || data == '.') && count < size)
				{
					myread(stream->fd, &data, sizeof(char));
					buff[count++] = data;
				}
				mylseek(stream->fd, -1, SEEK_CUR);
				buff[count] = '\0';
				*(float *)va_arg(arguments, float *) = atof(buff);
			}
		}
	}

	va_end(arguments);
}

int myfprintf(myFILE *restrict stream, const char *restrict format, ...)
{
	va_list arguments;

	va_start(arguments, format);

	char tmp;

	for (size_t i = 0; i < strlen(format) - 1; ++i)
	{

		if (format[i] != '%')
		{
			if ((i > 0 && format[i - 1] != '%') || i == 0)
			{
				mywrite(stream->fd, &format[i], sizeof(char));
			}
		}
		if (format[i] == '%')
		{
			if (format[i + 1] == 'd')
			{
				int num = va_arg(arguments, int);
				char buff[20];
				sprintf(buff, "%d", num);

				mywrite(stream->fd, buff, strlen(buff));
			}
			else if (format[i + 1] == 'c')
			{
				char num = va_arg(arguments, int);
				mywrite(stream->fd, &num, sizeof(char));
			}
			else if (format[i + 1] == 'f')
			{
				float num = va_arg(arguments, double);
				char buff[20];
				sprintf(buff, "%f", num);

				mywrite(stream->fd, buff, strlen(buff));
			}
		}
	}

	va_end(arguments);
}
