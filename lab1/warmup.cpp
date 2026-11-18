// Пункт 1.1 «Разминка»: код из задания без изменений.
#include <iostream>
#include <string>

using namespace std;

int main()
{
    string s = "01234";
    for (unsigned int i = s.size() - 1; i >= 0; i--)
    {
        std::cout << s[i] << std::endl;
    }
}
