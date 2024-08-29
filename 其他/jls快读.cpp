namespace iof {
    const int Buffer_MAXSIZE = 0xfffff;

    const char endl = '\n';

    class ifast {
    public:
        ifast &operator>>(char &ch);

        ifast &operator>>(char *s);

        ifast &operator>>(std::string &string);

        ifast &operator>>(bool &x) { return read_unsigned_integer(x); }

        ifast &operator>>(int &x) { return read_signed_integer(x); }

        ifast &operator>>(long long &x) { return read_signed_integer(x); }

        ifast &operator>>(unsigned int &x) { return read_unsigned_integer(x); }

        ifast &operator>>(unsigned long long &x) { return read_unsigned_integer(x); }

        void getline(std::string &string);

        void getline(char *s);

        auto hasMoreToken() { return front <= back; }

    private:
        template<typename T>
        ifast &read_signed_integer(T &x);

        template<typename T>
        ifast &read_unsigned_integer(T &x);

        char getchar();

        char buffer[Buffer_MAXSIZE] {};

        char *front = buffer;

        char *back = buffer;

        char next_char{};
    } fin;

    class ofast {
    public:
        ofast &operator<<(const char &c);

        ofast &operator<<(const char *c);

        ofast &operator<<(const std::string &s);

        ofast &operator<<(const bool &x) { return print_integer(x); }

        ofast &operator<<(const int &x) { return print_integer(x); }

        ofast &operator<<(const long long &x) { return print_integer(x); }

        ofast &operator<<(const unsigned int &x) { return print_integer(x); }

        ofast &operator<<(const unsigned long long &x) { return print_integer(x); }

        void flush();

        ~ofast() { flush(); }

    private:
        void putchar(const char &c) { *top++ = c; }

        char buffer[Buffer_MAXSIZE] {};

        char *top = buffer;

        template<typename T>
        ofast &print_positive_integer(const T &x);

        template<typename T>
        ofast &print_integer(const T &x);
    } fout;

    char ifast::getchar() {
        if (front == back) {
            back = buffer + fread(buffer, 1, Buffer_MAXSIZE, stdin);
            front = buffer;
        }
        return *(front++);
    }

    void ifast::getline(std::string &string) {
        string.clear();
        for (*this >> next_char; hasMoreToken() && next_char != '\n'; next_char = getchar()) {
            string.push_back(next_char);
        }
    }

    void ifast::getline(char *s) {
        for (char *p = s; hasMoreToken() && next_char != '\n'; next_char = getchar(), p++) {
            *p = next_char;
        }
    }

    ifast &ifast::operator>>(char &ch) {
        do {
            ch = getchar();
        } while (hasMoreToken() && (ch == ' ' || ch == '\n'));
        return *this;
    }

    ifast &ifast::operator>>(char *s) {
        *this >> next_char;
        for (char *p = s; hasMoreToken() && next_char != ' ' && next_char != '\n'; next_char = getchar(), p++) {
            *p = next_char;
        }
        return *this;
    }

    ifast &ifast::operator>>(std::string &string) {
        *this >> next_char;
        string.clear();
        for (; hasMoreToken() && next_char != ' ' && next_char != '\n'; next_char = getchar()) {
            string.push_back(next_char);
        }
        return *this;
    }

    template<typename T>
    ifast &ifast::read_unsigned_integer(T &x) {
        *this >> next_char;
        x = 0;
        do {
            x = (x << 1) + (x << 3) + next_char - '0';
            next_char = getchar();
        } while (hasMoreToken() && next_char != ' ' && next_char != '\n');
        return *this;
    }

    template<typename T>
    ifast &ifast::read_signed_integer(T &x) {
        *this >> next_char;
        auto isNegative = next_char == '-';
        if (isNegative) {
            next_char = getchar();
        }
        x = 0;
        do {
            x = (x << 1) + (x << 3) + next_char - '0';
            next_char = getchar();
        } while (hasMoreToken() && next_char != ' ' && next_char != '\n');
        if (isNegative) {
            x = -x;
        }
        return *this;
    }

    ofast &ofast::operator<<(const char &c) {
        putchar(c);
        return *this;
    }

    ofast &ofast::operator<<(const char *c) {
        for (auto *p = c; *p != '\0'; p++) {
            putchar(*p);
        }
        return *this;
    }

    ofast &ofast::operator<<(const std::string &s) {
        for (const auto &i : s) {
            putchar(i);
        }
        return *this;
    }

    void ofast::flush() {
        fwrite(buffer, 1, top - buffer, stdout);
        top = buffer;
    }

    template<typename T>
    ofast &ofast::print_integer(const T &x) {
        if (x > 0) {
            return print_positive_integer(x);
        } else if (x == 0) {
            putchar('0');
            return *this;
        } else {
            putchar('-');
            return print_positive_integer(-x);
        }
    }

    template<typename T>
    ofast &ofast::print_positive_integer(const T &x) {
        if (x > 9) {
            print_positive_integer(x / 10);
        }
        putchar(x % 10 + '0');
        return *this;
    }
}
#define cin myCin
#define cout myCout
iof::ifast myCin;
iof::ofast myCout;