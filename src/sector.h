#ifndef SECTOR_H
#define SECTOR_H

class Sector
{
public:
    Sector();

public:
    double fi;
    double alpha;
    int di;
    int dj;
    bool blocked;
    double dfi;

public:
    void set_num(int num);
    void set_dfi(double fi0);
    double calc_fi(double x, double y);
    bool inrange(int nx, int ny, int a, int b);
};

#endif // SECTOR_H
