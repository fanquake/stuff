// gcc -flto -frandom-seed=1 -c lto_file_paths.c
// -c because we only want to compile and assemble
// file path should be in .gnu.lto_initialized section
//
// objdump -s lto_file_paths.o | grep "gnu.lto_initialized" -A10
//
// Contents of section .gnu.lto_initialized.0.1:
//  0000 28b52ffd 20529102 00200000 00260000  (./. R... ...&..
//  0010 00000000 00050127 0286087f 05018a01  .......'........
//  0020 868080fc d7c59601 7dbb0100 3f828880  ........}...?...
//  0030 047b79bc 0100046e 696c000d 2f686f6d  .{y....nil../hom
//  0040 652f7562 756e7475 00116c74 6f5f6669  e/ubuntu..lto_fi
//  0050 6c655f70 61746873 2e6300             le_paths.c.     
// Contents of section .gnu.lto_main.1.1:
//  0000 28b52ffd 60310065 0700824d 3135506d  (./.`1.e...M15Pm
//  0010 d3016080 22862cb1 72aadc82 794ae193  ..`.".,.r...yJ..
//  0020 67c6f36a 8e3e5492 6f81b798 236e1b83  g..j.>T.o...#n..

// Looks like this could be related too:
// https://gcc.gnu.org/pipermail/gcc-patches/2022-November/606205.html

struct A {
	const char *name;
};

struct A initialized = {
	"nil"
};

int main() {
	return 0;
}
