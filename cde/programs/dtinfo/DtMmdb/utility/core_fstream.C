/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
// $XConsortium: core_fstream.C /main/4 1996/10/04 10:47:25 drk $

#include <stdlib.h>
#include <string.h>

void filebuf_exam_test1()
{
   fprintf(stderr, "fstream test1 : read\n");
   fstream fb("stream_test.read", ios::in|ios::out);
   char buf[10];

   for ( int i=0; i<2; i++ ) {
      fb.read(buf, 4);
      fprintf(stderr, "buf[0]=%c\n", buf[0]);
      fprintf(stderr, "buf[1]=%c\n", buf[1]);
      fprintf(stderr, "buf[2]=%c\n", buf[2]);
      fprintf(stderr, "buf[3]=%c\n", buf[3]);

      fprintf(stderr, "gcount=%d\n", fb.gcount());
   }
}

void filebuf_exam_test2()
{
   fprintf(stderr, "fstream test2 : write \n");
   fstream fb("/tmp/stream_test.write", ios::in|ios::out|ios::trunc);
   char* buf = "abcdefghijklmnopq";

   for ( int i=0; i<2; i++ ) {
      fb.write(buf+i*4, 4);
   }

   system("cmp stream_test.write /tmp/stream_test.write");
   system("rm /tmp/stream_test.write");
}

void filebuf_exam_test3()
{
   fprintf(stderr, "fstream test3 : getline\n");
   fstream fb("stream_test.getline", ios::in|ios::out);
   char buf[100];

   while ( fb.getline(buf, 100) ) {
      fprintf(stderr, "buf=%s\n", buf);
      fprintf(stderr, "gcount=%d\n", fb.gcount());
   }
}

void filebuf_exam_test4()
{
   fprintf(stderr, "fstream test4 : word counting\n");
   fstream fb("stream_test.mobydick", ios::in);

   char buf[50];
   char largest[50];
   int curlen, max = -1, cnt = 0;
   while ( fb >> buf ) {
     curlen = strlen(buf);
     ++cnt;
     if ( curlen > max ) {
       max = curlen;
       memcpy(largest, buf, strlen(buf));
     }
   }
   fprintf(stderr, "# of words read is = %d\n", cnt);
   fprintf(stderr, "the longest word has a length of = %d\n", max);
   fprintf(stderr, "the longest word is = %s\n", largest);
}

void filebuf_exam_test5()
{
   fprintf(stderr, "fstream test 5: (extraction)\n");
   fstream fb("stream_test.extraction", ios::in|ios::out);

   int x;
   long y;
   char c;
   unsigned short u;
   unsigned int v;
// read 
// int long char (u)short (u)int

   fb >> x; fprintf(stderr, "x=%d\n", x);
   fprintf(stderr, "gcount=%d\n", fb.gcount());

   fb >> y; fprintf(stderr, "y=%ld\n", y);
   fprintf(stderr, "gcount=%d\n", fb.gcount());

   fb >> c; fprintf(stderr, "c=%c\n", c);
   fprintf(stderr, "gcount=%d\n", fb.gcount());

   fb >> u; fprintf(stderr, "u=%d\n", u);
   fprintf(stderr, "gcount=%d\n", fb.gcount());

   fb >> v; fprintf(stderr, "v=%d\n", v);
   fprintf(stderr, "gcount=%d\n", fb.gcount());

}

void filebuf_exam_test6()
{
   fprintf(stderr, "fstream test 6: (mixed insertion and extraction [1]) \n");

   system("cp stream_test.mixed_ins_extr /tmp/stream_test.mixed_ins_extr");

   fstream fb("/tmp/stream_test.mixed_ins_extr", ios::in|ios::out);

   int x;
   long y;
   char c;
   unsigned short u;
   unsigned int v;
// read
// int long char (u)short (u)int

   fb >> x; fprintf(stderr, "x=%d\n", x);
   fb >> y; fprintf(stderr, "y=%ld\n", y);

   c = '0'; fb << c; 
   c = '1'; fb << c; 

   system("diff stream_test.mixed_ins_extr /tmp/stream_test.mixed_ins_extr");
}

void filebuf_exam_test7()
{
   fprintf(stderr, "fstream test 7: (mixed insertion and extraction [2]) \n");

   system("cp stream_test.mixed_ins_extr.2 /tmp/stream_test.mixed_ins_extr.2");
   fstream fb("/tmp/stream_test.mixed_ins_extr.2", ios::in|ios::out|ios::trunc);

   int x = 1;
   long y = 2;
   char c;

   fb << x << y << "abcdefghijk";

   fb.seekg(9, ios::beg);

   fb >> c; fprintf(stderr, "c=%c\n", c);
   fb >> c; fprintf(stderr, "c=%c\n", c);

   system("diff /tmp/stream_test.mixed_ins_extr.2 stream_test.mixed_ins_extr.2");
   system("rm /tmp/stream_test.mixed_ins_extr.2");
}

void filebuf_exam_test8()
{
   fprintf(stderr, "fstream test 8: (seek and read) \n");

   fstream fb("stream_test.seek_and_read", ios::in|ios::out);

   fb.seekg(10, ios::beg);

   char c;
   fb >> c; fprintf(stderr, "c=%c\n", c);
   fb >> c; fprintf(stderr, "c=%c\n", c);

   fb.seekg(20, ios::beg);
   fb >> c; fprintf(stderr, "c=%c\n", c);
   fb >> c; fprintf(stderr, "c=%c\n", c);
}

void filebuf_exam_test9()
{
   fprintf(stderr, "fstream test 9: (append) \n");

   system("rm -f /tmp/stream_test.append");
   fstream fb("/tmp/stream_test.append", ios::app);

   fb << "lseek() sets the seek"; 
   fb << " pointer associated with the open"; 

   system("diff /tmp/stream_test.append stream_test.append");
   system("rm /tmp/stream_test.append");
}

void filebuf_exam_test10()
{
   fprintf(stderr, "fstream test 10: (cerr) \n");

   fstream fb(2);

   fb << "cerr: lseek() sets the seek\n"; 
   fb << "cerr:  pointer associated with the open\n"; 

   fstream fb1(1);
   fb1 << "cout::lseek() sets the seek\n"; 
   fb1 << "cout:: pointer associated with the open\n"; 
}

void filebuf_exam_test11(char* nm)
{
   fprintf(stderr, "fstream test11 : getline (2)\n");
   fstream fb(nm, ios::in);
   char buf[100];

   while ( fb.getline(buf, 100) ) {
      fprintf(stderr, "buf=%s\n", buf);
      fprintf(stderr, "gcount=%d\n", fb.gcount());
   }
}

void filebuf_exam_test12()
{
   fprintf(stderr, "fstream test12 : write (2)\n");
   fstream fb("/tmp/stream_test.write", ios::out);
   char* buf = "abcdefghijklmnopq";

   for ( int i=0; i<2; i++ ) {
      fb.write(buf+i*4, 4);
   }
   system("diff stream_test.write /tmp/stream_test.write");
   system("rm /tmp/stream_test.write");
}

void filebuf_exam_test13(char* nm, int pos)
{
   fprintf(stderr, "fstream test13 : read (2)\n");
   fstream fb(nm, ios::in);
   char buf[8192];

   fb.seekg(pos, ios::beg);

   int len = 8192;
   fb.read(buf, len);

   fprintf(stderr, "len=%d\n", len);
   fprintf(stderr, "gcount=%d\n", fb.gcount());
}

void filebuf_exam_test14(char* nm)
{
   fprintf(stderr, "fstream test14 : write (3)\n");
   fstream fb(nm, ios::out);
   char* buf = "abcdefghijklmnopq";

   for ( int i=0; i<2; i++ ) {
      fb.write(buf+i*4, 4);
   }
}


void usage(char** argv)
{
      fprintf(stderr, "Usage: %s option\n", argv[0]);
      fprintf(stderr, "option = 1: read\n");
      fprintf(stderr, "         2: write\n");
      fprintf(stderr, "         3: getline\n");
      fprintf(stderr, "         4: word counting\n");
      fprintf(stderr, "         5: extraction\n");
      fprintf(stderr, "         6: mixed insertion/extraction [1]\n");
      fprintf(stderr, "         7: mixed insertion/extraction [2]\n");
      fprintf(stderr, "         8: seek and read\n");
      fprintf(stderr, "         9: append\n");
      fprintf(stderr, "         10: cerr\n");
      fprintf(stderr, "         11: getline (2). arguments: 11 filename\n");
      fprintf(stderr, "         12: write (2)\n");
      fprintf(stderr, "         13: read (2). arg: 13 filename offset\n");
      fprintf(stderr, "         14: write (3). arg: 14 filename\n");
}

int main(int argc, char** argv)
{
   if ( argc <= 1 ) {
      usage(argv);
      return 1;
   }

   int i = atoi(argv[1]);
   switch (i) {
     case 1:
      filebuf_exam_test1();
      break;
     case 2:
      filebuf_exam_test2();
      break;
     case 3:
      filebuf_exam_test3();
      break;
     case 4:
      filebuf_exam_test4();
      break;
     case 5:
      filebuf_exam_test5();
      break;
     case 6:
      filebuf_exam_test6();
      break;
     case 7:
      filebuf_exam_test7();
      break;
     case 8:
      filebuf_exam_test8();
      break;
     case 9:
      filebuf_exam_test9();
      break;
     case 10:
      filebuf_exam_test10();
      break;
     case 11:
      if ( argc == 3 )
         filebuf_exam_test11(argv[2]);
      else
         usage(argv);
      break;
     case 12:
      filebuf_exam_test12();
      break;
     case 13:
      if ( argc == 4 )
         filebuf_exam_test13(argv[2], atoi(argv[3]));
      break;
     case 14:
      if ( argc == 3 )
         filebuf_exam_test14(argv[2]);
      break;
     default:
      filebuf_exam_test1();
      filebuf_exam_test2();
      filebuf_exam_test3();
      filebuf_exam_test4();
      filebuf_exam_test5();
      filebuf_exam_test6();
      filebuf_exam_test7();
      filebuf_exam_test8();
      filebuf_exam_test9();
      filebuf_exam_test10();
      filebuf_exam_test12();
   }
   return 0;
}
