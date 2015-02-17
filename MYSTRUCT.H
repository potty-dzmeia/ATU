



struct tunetype {
	int c1;
	int c2;
	int l;
	int swr;
} Tune[9];
//----------

struct bandtype {
    struct tunetype Tune[10];
}Band[8];

//----------


struct anttype{

	struct bandtype Band[9];
}Ant[6];



//
struct bandche{
	int lastCw[2];
}lastBand[9];
struct antenka{//structura zapametqvashta kmbinaciqta ot Band+Ant+Mode na kakuv # iTune e ravno
	struct bandche lastBand[9];
} lastAnt[6];
//--------------


typedef unsigned long int inty64;




