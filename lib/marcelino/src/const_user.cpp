#include "const_user.hpp"


namespace math
{

float pow(int base, int expoent) {
    if(expoent == 0) return 1;
    if(expoent == 1) return base;
    float acumulador = 1.0f;
    if(expoent > 1) {
        while(expoent--) acumulador *= base;
        return acumulador;
    }
    expoent = -expoent;
    while(expoent--) acumulador *= base;
    return 1/acumulador;
}

} // namespace math