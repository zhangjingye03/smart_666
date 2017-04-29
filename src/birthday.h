// total count
#define BIRMEN 1
// name, GBK
const char* birName[] = {"\xC9\xB5\xB1\xC6"};
// sex, 1->girl, 0->boy
const char birSex[] = {1};
// month
const char birMonth[] = {2};
// day
const char birDay[] = {31};

char getTodayBir (char mon, char day) {
  for (char i = 0; i < BIRMEN; i++) {
    if (mon == birMonth[i] && day == birDay[i]) return i;
  }
  return -1;
}
