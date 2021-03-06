#define BANNER "This is program vlna, version 1.5, (c) 1995, 2002, 2009, 2010 Petr Olsak\n" \

#define OK 0
#define WARNING 1
#define IO_ERR 2
#define BAD_OPTIONS 3
#define BAD_PROGRAM 4
#define MAXLEN 120
#define ONE 1
#define ANY 2
#define ONE_NOT -1
#define ANY_NOT -2 \

#define MAXPATT 200
#define MAXBUFF 500
#define BUFI 300
#define FOUND -1
#define NOFOUND -2
#define TEXTMODE 0
#define MATHMODE 1
#define DISPLAYMODE 2
#define VERBMODE 3
/*1:*/
#line 19 "./vlna.w"

/*3:*/
#line 39 "./vlna.w"

#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 

/*:3*/
#line 20 "./vlna.w"

/*4:*/
#line 56 "./vlna.w"

char*prog_name;
int status;

/*:4*//*7:*/
#line 138 "./vlna.w"

int isfilter= 0,silent= 0,rmbackup= 0,nomath= 0,noverb= 0,web= 0,latex= 0;
char charsetdefault[]= "KkSsVvZzOoUuAI";
char*charset= charsetdefault;

/*:7*//*8:*/
#line 148 "./vlna.w"

unsigned char tiestr[MAXLEN];
int tiestrlen;

/*:8*//*17:*/
#line 330 "./vlna.w"

typedef struct PATITEM{
char*str;
int flag;
struct PATITEM*next;
}PATITEM;
typedef struct PATTERN{
PATITEM*patt;
void(*proc)();
struct PATTERN*next;
}PATTERN;

/*:17*//*18:*/
#line 382 "./vlna.w"

PATITEM*lapi[MAXPATT];
PATTERN*lapt[MAXPATT];
PATTERN*listpatt,*normallist,*commentlist,*pt,*lastpt= NULL;
PATITEM*pi,*lastpi= NULL;
char c;
char buff[MAXBUFF];
int ind;

/*:18*//*19:*/
#line 401 "./vlna.w"

char buffnz[8];
int inz;

/*:19*//*23:*/
#line 470 "./vlna.w"

char strings[512];
int i;

/*:23*//*28:*/
#line 558 "./vlna.w"

char*filename;
long int numline,numchanges;
int mode;

/*:28*//*36:*/
#line 697 "./vlna.w"

char tblanks[]= " ~\t";
char blanks[]= " \t";
char blankscr[]= " \t\n";
char tblankscr[]= " ~\t\n";
char nochar[]= "%~\n";
char cr[]= "\n";
char prefixes[]= "[({~";
char dolar[]= "$";
char backslash[]= "\\";
char openbrace[]= "{";
char letters[]= "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
PATTERN*vlnkalist,*mathlist,*parcheck,*verblist;

/*:36*/
#line 21 "./vlna.w"

/*6:*/
#line 114 "./vlna.w"

void printusage()
{
fprintf(stderr,
"usage: vlna [opt] [filenames]\n"
"  opt -f :  filter mode: file1 file2 ... file1->file2\n"
"                         file1       ... file1->stdout\n"
"                                     ... stdin->stdout\n"
"            nofilter: file1 [file2 file3 ...] all are in/out\n"
"      -s :  silent: no messages to stderr\n"
"      -r :  rmbackup: if nofilter, removes temporary files\n"
"      -v charset :  set of lettres to add tie, default: KkSsVvZzOoUuAI\n"
"      -x code : code for tie symbol, default: 7E, example -x C2A0\n"
"      -m :  nomath: ignores math modes\n"
"      -n :  noverb: ignores verbatim modes\n"
"      -l :  LaTeX mode\n"
"      -w :  web mode\n");
}

/*:6*//*10:*/
#line 176 "./vlna.w"

unsigned char hexnum(char c){
if(c>='0'&&c<='9')return c-'0';
if(c>='A'&&c<='F')return c-'A'+10;
if(c>='a'&&c<='f')return c-'a'+10;
printusage(),exit(BAD_OPTIONS);
}
void settiestr(char*s){
int i,j,c;
i= strlen(s);
if((i> 2*MAXLEN)||i%2==1)printusage(),exit(BAD_OPTIONS);
tiestrlen= i/2;
j= 0;
for(i= 0;i<tiestrlen;i++){
tiestr[i]= hexnum(s[j++])<<4;
tiestr[i]+= hexnum(s[j++]);
}
}

/*:10*//*12:*/
#line 206 "./vlna.w"

void ioerr(f)
char*f;
{
fprintf(stderr,"%s: cannot open file %s\n",prog_name,f);
}

/*:12*//*20:*/
#line 409 "./vlna.w"

void*myalloc(size)
int size;
{
void*p;
p= malloc(size);
if(p==NULL)
{
fprintf(stderr,"%s, no memory, malloc failed\n",prog_name);
exit(BAD_PROGRAM);
}
return p;
}

/*:20*//*21:*/
#line 427 "./vlna.w"

PATTERN*setpattern(proc)
void(*proc)();
{
PATTERN*pp;
pp= myalloc(sizeof(PATTERN));
pp->proc= proc;
pp->next= NULL;
pp->patt= NULL;
if(lastpt!=NULL)lastpt->next= pp;
lastpt= pp;
lastpi= NULL;
return pp;
}

/*:21*//*22:*/
#line 446 "./vlna.w"

void setpi(str,flag)
char*str;
int flag;
{
PATITEM*p;
p= myalloc(sizeof(PATITEM));
p->str= str;p->flag= flag;
p->next= NULL;
if(lastpi==NULL)lastpt->patt= p;
else lastpi->next= p;
lastpi= p;
}

/*:22*//*25:*/
#line 481 "./vlna.w"

PATTERN*normalpattern(proc,str)
void(*proc)();
char*str;
{
PATTERN*pp;
int j= 0;
pp= setpattern(proc);
while(str[j]){
if(str[j]=='.'){
j++;
if(str[j]!='.'){
setpi(blankscr,ANY);
continue;
}
}
setpi(&strings[(unsigned char)str[j]*2],ONE);
j++;
}
return pp;
}

/*:25*//*26:*/
#line 519 "./vlna.w"

int match(p)
PATITEM*p;
{
int m;
if(strchr(p->str,c)!=NULL)m= 1;
else m= -1;
switch(m*p->flag){
case ANY:return 0;
case ONE:if(p->next==NULL)return FOUND;
return 1;
case ONE_NOT:return NOFOUND;
case ANY_NOT:/*27:*/
#line 538 "./vlna.w"

switch(m= match(p->next)){
case NOFOUND:return NOFOUND;
case FOUND:return FOUND;
default:return 1+m;
}

/*:27*/
#line 531 "./vlna.w"
;
}
return 0;
}

/*:26*//*39:*/
#line 738 "./vlna.w"

void vlnkain()
{
int i;
char p,z;
ind--;
p= buff[ind--];
z= buff[ind];
while(!buff[ind]||(strchr(blanks,buff[ind])!=NULL))ind--;
if(!buff[++ind]&&tiestr[0])ind++;
for(i= 0;i<tiestrlen;i++)buff[ind++]= tiestr[i];
i= tiestrlen;
if(!tiestr[0])i--;
if(!z&&(i%2))buff[ind++]= 0;
buff[ind++]= p;
numchanges++;
}

/*:39*//*41:*/
#line 782 "./vlna.w"

void vlnkacr()
{
char p,z;
int i,j;
ind--;
p= buff[ind--];
z= buff[ind];
while(!buff[ind]||(strchr(blankscr,buff[ind])!=NULL))ind--;
i= ind;
while(i>=0&&(strchr(blankscr,buff[i])==NULL))i--;
j= i;
while(i>=0&&(!buff[ind]||(strchr(blanks,buff[i])!=NULL)))i--;
if(i>=0&&buff[i]=='\n')j= -1;
if(j>=0)buff[j]= '\n';
else numline--;
if(!buff[++ind]&&tiestr[0])ind++;
for(i= 0;i<tiestrlen;i++)buff[ind++]= tiestr[i];
i= tiestrlen;
if(!tiestr[0])i--;
if(!z&&(i%2))buff[ind++]= 0;
buff[ind++]= p;
numchanges++;
}

/*:41*//*44:*/
#line 852 "./vlna.w"

void tielock()
{
c= 1;
}

/*:44*//*45:*/
#line 861 "./vlna.w"

void printwarning()
{
if(!silent)
fprintf(stderr,
"~!~ warning: text/math/verb mode mismatch,  file: %s,  line: %ld\n",
filename,numline-(c=='\n'?1:0));
status= WARNING;
}

/*:45*//*47:*/
#line 886 "./vlna.w"

void mathin()
{
if(mode!=TEXTMODE)printwarning();
mode= MATHMODE;
normallist= listpatt= mathlist;
}
void mathout()
{
if(mode!=MATHMODE)printwarning();
mode= TEXTMODE;
normallist= listpatt= vlnkalist;
}

/*:47*//*48:*/
#line 904 "./vlna.w"

void onedollar()
{
if(buffnz[inz-3]=='\\'||(buffnz[inz-3]=='$'&&buffnz[inz-4]!='\\'))return;
if(mode==DISPLAYMODE)printwarning();
else{
if(mode==TEXTMODE)mathin();
else mathout();
}
}

/*:48*//*50:*/
#line 923 "./vlna.w"

void checkmode()
{
if(mode!=TEXTMODE){
printwarning();
mode= TEXTMODE;
normallist= listpatt= vlnkalist;
}
}

/*:50*//*52:*/
#line 949 "./vlna.w"

void displayin()
{
if(mode!=TEXTMODE)printwarning();
mode= DISPLAYMODE;normallist= listpatt= parcheck;
}
void displayout()
{
if(mode!=DISPLAYMODE)printwarning();
mode= TEXTMODE;normallist= listpatt= vlnkalist;
}
void twodollars()
{
if(buffnz[inz-3]=='\\')return;
if(mode==DISPLAYMODE)displayout();
else displayin();
}

/*:52*//*54:*/
#line 998 "./vlna.w"

int prevmode;
PATTERN*prevlist,*verboutlist[4];
char verbchar[2];
void verbinchar()
{
prevmode= mode;
verbchar[0]= c;
c= 1;
listpatt= normallist= verboutlist[0];
prevlist= listpatt->next;
listpatt->next= NULL;
mode= VERBMODE;
}

/*:54*//*55:*/
#line 1019 "./vlna.w"

void verbin()
{
int i;
i= 0;
prevmode= mode;
switch(c){
case't':i= 1;break;
case'm':i= 2;break;
case'<':;
case'd':i= 3;
if(buffnz[inz-3]=='@')return;
break;
}
listpatt= normallist= verboutlist[i];
prevlist= listpatt->next;
if(c!='<'&&c!='d')listpatt->next= NULL;
mode= VERBMODE;
}

/*:55*//*56:*/
#line 1039 "./vlna.w"

void verbout()
{
if(mode!=VERBMODE)return;
if(web&&buffnz[inz-2]=='@'&&buffnz[inz-3]=='@')return;
mode= prevmode;
normallist->next= prevlist;
switch(mode){
case DISPLAYMODE:normallist= listpatt= parcheck;break;
case MATHMODE:normallist= listpatt= mathlist;break;
case TEXTMODE:normallist= listpatt= vlnkalist;break;
}
}

/*:56*//*58:*/
#line 1061 "./vlna.w"

void tieoff()
{
normallist= NULL;
}
void tieon()
{
normallist= vlnkalist;
}

/*:58*/
#line 22 "./vlna.w"

/*29:*/
#line 567 "./vlna.w"

void tie(input,output)
FILE*input,*output;
{
int ap;
register int k,m,n;
int ic;
PATTERN*pp;
PATITEM*pi;

/*30:*/
#line 604 "./vlna.w"

for(k= 0;k<MAXPATT;k++)lapi[k]= NULL;
c= '\n';
buff[0]= 1;mode= ap= 0;ind= 1;
for(inz= 0;inz<4;inz++)buffnz[inz]= 0;
inz= 4;
numline= 1;numchanges= 0;
mode= TEXTMODE;

/*:30*//*38:*/
#line 727 "./vlna.w"

listpatt= normallist= vlnkalist;

/*:38*/
#line 577 "./vlna.w"
;

while(!feof(input)){
if(ap==0&&ind> BUFI&&c!='\\')/*31:*/
#line 622 "./vlna.w"

{
fwrite(&buff[1],ind-1,1,output);
ind= 1;
}

/*:31*/
#line 580 "./vlna.w"
;
/*33:*/
#line 651 "./vlna.w"

pp= listpatt;
if(c)while(pp!=NULL){
switch(m= match(pp->patt)){
case FOUND:(*pp->proc)();
case NOFOUND:break;
default:/*34:*/
#line 666 "./vlna.w"

pi= pp->patt;
while(m--)pi= pi->next;
n= ap;k= 0;
while(n){
if(lapi[k]==pi)break;
if(lapi[k++]!=NULL)n--;
}
if(!n){
k= 0;
while(lapi[k]!=NULL)k++;
if(k>=MAXPATT){
fprintf(stderr,"I cannot allocate pp, is anything wrong?\n");
exit(BAD_PROGRAM);
}
lapt[k]= pp;lapi[k]= pi;ap++;
}

/*:34*/
#line 657 "./vlna.w"
;
}
pp= pp->next;
}

/*:33*/
#line 581 "./vlna.w"
;
if(ind>=MAXBUFF){
fprintf(stderr,"Operating buffer overflow, is anything wrong?\n");
exit(BAD_PROGRAM);
}
if((ic= getc(input))==EOF)
break;
buff[ind++]= c= ic;
if(c==0)continue;
if(inz>=8){
for(inz= 0;inz<4;inz++)buffnz[inz]= buffnz[inz+4];
inz= 4;
}
buffnz[inz++]= c;
if(c=='\n')numline++,listpatt= normallist;
if(c=='%'&&mode!=VERBMODE&&buffnz[inz-2]!='\\')listpatt= commentlist;
/*32:*/
#line 634 "./vlna.w"

n= ap;k= 0;
while(n){
while(lapi[k]==NULL)k++;
switch(m= match(lapi[k])){
case FOUND:(*lapt[k]->proc)();
case NOFOUND:lapi[k]= NULL;
ap--;break;
default:while(m--)lapi[k]= lapi[k]->next;
}
k++;n--;
}

/*:32*/
#line 597 "./vlna.w"
;
}
/*31:*/
#line 622 "./vlna.w"

{
fwrite(&buff[1],ind-1,1,output);
ind= 1;
}

/*:31*/
#line 599 "./vlna.w"
;
if(!web)checkmode();
if(!silent)/*35:*/
#line 685 "./vlna.w"

fprintf(stderr,"~~~ file: %s\t  lines: %ld, changes: %ld\n",
filename,numline,numchanges);

/*:35*/
#line 601 "./vlna.w"
;
}

/*:29*/
#line 23 "./vlna.w"

/*5:*/
#line 61 "./vlna.w"

int main(argc,argv)
int argc;
char**argv;
{
/*11:*/
#line 199 "./vlna.w"

FILE*infile,*outfile;
char backup[MAXLEN];
int j;

/*:11*/
#line 66 "./vlna.w"
;
prog_name= argv[0];status= OK;
/*9:*/
#line 152 "./vlna.w"

tiestr[0]= '~';
tiestrlen= 1;
while(argc> 1&&argv[1][0]=='-'){
if(argv[1][2]!=0)printusage(),exit(BAD_OPTIONS);
switch(argv[1][1]){
case'f':isfilter= 1;break;
case's':silent= 1;break;
case'r':rmbackup= 1;break;
case'v':if(argc<2)printusage(),exit(BAD_OPTIONS);
argv++;argc--;charset= argv[1];break;
case'x':if(argc<2)printusage(),exit(BAD_OPTIONS);
argv++;argc--;settiestr(argv[1]);break;
case'm':nomath= 1;break;
case'n':noverb= 1;break;
case'l':latex= 1;break;
case'w':web= 1;break;
default:printusage(),exit(BAD_OPTIONS);

}
argc--;argv++;
}

/*:9*/
#line 68 "./vlna.w"
;
if(!silent)fprintf(stderr,BANNER);
/*24:*/
#line 475 "./vlna.w"

for(i= 0;i<256;i++){
strings[2*i]= (char)i;strings[2*i+1]= 0;
}

/*:24*//*37:*/
#line 717 "./vlna.w"

vlnkalist= setpattern(vlnkain);
setpi(tblankscr,ONE);
setpi(tblanks,ANY);
setpi(prefixes,ANY);
setpi(charset,ONE);
setpi(blanks,ONE);
setpi(blanks,ANY);
setpi(nochar,ONE_NOT);

/*:37*//*40:*/
#line 758 "./vlna.w"

setpattern(vlnkacr);
setpi(tblankscr,ONE);
setpi(tblanks,ANY);
setpi(prefixes,ANY);
setpi(charset,ONE);
setpi(blanks,ANY);
setpi(cr,ONE);
setpi(blanks,ANY);
setpi(nochar,ONE_NOT);

/*:40*//*42:*/
#line 808 "./vlna.w"

setpattern(vlnkain);
setpi(tblankscr,ONE);
setpi(backslash,ONE);
setpi(letters,ONE);
setpi(letters,ANY);
setpi(openbrace,ONE);
setpi(prefixes,ANY);
setpi(charset,ONE);
setpi(blanks,ONE);
setpi(blanks,ANY);
setpi(nochar,ONE_NOT);

setpattern(vlnkacr);
setpi(tblankscr,ONE);
setpi(backslash,ONE);
setpi(letters,ONE);
setpi(letters,ANY);
setpi(openbrace,ONE);
setpi(prefixes,ANY);
setpi(charset,ONE);
setpi(blanks,ANY);
setpi(cr,ONE);
setpi(blanks,ANY);
setpi(nochar,ONE_NOT);



/*:42*//*43:*/
#line 841 "./vlna.w"

normalpattern(tielock,"\\TeX");
setpi(blankscr,ONE);
normalpattern(tielock,"\\LaTeX");
setpi(blankscr,ONE);

/*:43*//*46:*/
#line 875 "./vlna.w"

if(!nomath){
mathlist= setpattern(onedollar);
setpi(dolar,ONE);
setpi(dolar,ONE_NOT);
if(latex){
normalpattern(mathin,"\\begin.{math}");
normalpattern(mathout,"\\end.{math}");
}
}

/*:46*//*49:*/
#line 917 "./vlna.w"

parcheck= setpattern(checkmode);
setpi(cr,ONE);
setpi(blanks,ANY);
setpi(cr,ONE);

/*:49*//*51:*/
#line 938 "./vlna.w"

if(!nomath){
normalpattern(twodollars,"$$");
if(latex){
normalpattern(displayin,"\\begin.{displaymath");
normalpattern(displayin,"\\begin.{equation");
normalpattern(displayout,"\\end.{displaymath");
normalpattern(displayout,"\\end.{equation");
}
}

/*:51*//*53:*/
#line 970 "./vlna.w"

if(!noverb){
verblist= normalpattern(verbinchar,"\\verb");
setpi(blankscr,ANY);
setpi(blankscr,ONE_NOT);
normalpattern(verbin,"\\begtt");
if(latex)normalpattern(verbin,"\\begin.{verbatim");
}
if(web){
normalpattern(verbin,"@<");
normalpattern(verbin,"@d");
}
if(!noverb){
verboutlist[0]= setpattern(verbout);
setpi(verbchar,ONE);
verboutlist[1]= normalpattern(verbout,"\\endtt");
if(latex)verboutlist[2]= normalpattern(verbout,"\\end{verbatim");
}
if(web){
verboutlist[3]= normalpattern(verbout,"@ ");
normalpattern(verbout,"@*");
normalpattern(verbout,"@>|");
}


/*:53*//*57:*/
#line 1056 "./vlna.w"

lastpt= 0;
commentlist= normalpattern(tieoff,"%.~.-");
normalpattern(tieon,"%.~.+");

/*:57*/
#line 70 "./vlna.w"
;
/*13:*/
#line 214 "./vlna.w"

if(isfilter)/*14:*/
#line 226 "./vlna.w"

{
if(argc> 3)printusage(),exit(BAD_OPTIONS);
infile= stdin;outfile= stdout;
if(argc>=2)infile= fopen(argv[1],"r");
if(infile==NULL)ioerr(argv[1]),exit(IO_ERR);
if(argc==3)outfile= fopen(argv[2],"w");
if(outfile==NULL)ioerr(argv[2]),exit(IO_ERR);
if(argc>=2)filename= argv[1];
else filename= NULL;
tie(infile,outfile);
if(outfile!=stdout)fclose(outfile);
if(infile!=stdin)fclose(infile);
}

/*:14*/
#line 215 "./vlna.w"

else/*15:*/
#line 249 "./vlna.w"

{
if(argc==1)printusage(),exit(BAD_OPTIONS);
while(argc> 1){
argc--;argv++;
/*16:*/
#line 280 "./vlna.w"

infile= NULL;
j= strlen(argv[0])-1;
if(j>=MAXLEN||argv[0][j]=='~'){
if(!silent)fprintf(stderr,"%s: the conflict of file name %s\n",
prog_name,argv[0]);
}
else{
strcpy(backup,argv[0]);
backup[j]= '~';
remove(backup);
j= rename(argv[0],backup);
if(j==0)infile= fopen(backup,"r");
}

/*:16*/
#line 254 "./vlna.w"
;
if(infile==NULL){
ioerr(argv[0]);continue;
}
outfile= fopen(argv[0],"w");
if(outfile==NULL){
ioerr(argv[0]);
rename(backup,argv[0]);
status= WARNING;
continue;
}
filename= argv[0];
tie(infile,outfile);
fclose(outfile),fclose(infile);
if(rmbackup)remove(backup);
}
}

/*:15*/
#line 216 "./vlna.w"


/*:13*/
#line 71 "./vlna.w"
;
return status;
}

/*:5*/
#line 24 "./vlna.w"


/*:1*/
