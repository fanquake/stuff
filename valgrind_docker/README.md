# Valgrind in Docker

Valgrind (3.19.0 system package) in the CI system under Docker runs up to 5-6x
slower than valgrind (3.21.0) built from source.

This directory contains strace output from either Valgrind, when running inside
the CI system env, over the same fuzz target. i.e:

```bash
time FILE_ENV="./ci/test/00_setup_env_native_fuzz_with_valgrind.sh" ./ci/test_run_all.sh
```

to get a container, then exec in and:

```bash
export FUZZ=asmap_direct
strace -o sys_asmap_direct.txt valgrind --error-exitcode=1 \
	/home/ubuntu/ci_scratch/ci/scratch/build/bitcoin-x86_64-pc-linux-gnu/src/test/fuzz/fuzz \
	-runs=1 \
	/home/ubuntu/ci_scratch/ci/scratch/qa-assets/fuzz_seed_corpus/asmap_direct
```

`--quiet` was dropped from the Valgrind options (when stracing) to check versions. This can be
seen in the end of the output. i.e `Using Valgrind-3.xx`.

Both runs of the fuzzer iterate over all of the inputs, and seem to do the same
amount of work, however the built-from-source binary completes much faster. i.e:
```bash
Run asmap_direct with args ['valgrind', '--quiet', '--error-exitcode=1', '/home/ubuntu/ci_scratch/ci/scratch/build/bitcoin-x86_64-pc-linux-gnu/src/test/fuzz/fuzz', '-runs=1', PosixPath('/home/ubuntu/ci_scratch/ci/scratch/qa-assets/fuzz_seed_corpus/asmap_direct')]INFO: Running with entropic power schedule (0xFF, 100).
INFO: Seed: 4246728604
INFO: Loaded 1 modules   (248679 inline 8-bit counters): 248679 [0x281f998, 0x285c4ff), 
INFO: Loaded 1 PC tables (248679 PCs): 248679 [0x285c500,0x2c27b70), 
INFO:      645 files found in /home/ubuntu/ci_scratch/ci/scratch/qa-assets/fuzz_seed_corpus/asmap_direct
INFO: -max_len is not provided; libFuzzer will not generate inputs larger than 1048576 bytes
INFO: seed corpus: files: 645 min: 1b max: 1048576b total: 15304310b rss: 167Mb
#512	pulse  cov: 329 ft: 1557 corp: 233/22Kb exec/s: 18 rss: 187Mb
#646	INITED cov: 329 ft: 1576 corp: 247/39Kb exec/s: 4 rss: 215Mb
#646	DONE   cov: 329 ft: 1576 corp: 247/39Kb lim: 2177 exec/s: 4 rss: 215Mb
Done 646 runs in 141 second(s)
```

vs built-from-source
```bash
Run asmap_direct with args ['valgrind', '--quiet', '--error-exitcode=1', '/home/ubuntu/ci_scratch/ci/scratch/build/bitcoin-x86_64-pc-linux-gnu/src/test/fuzz/fuzz', '-runs=1', PosixPath('/home/ubuntu/ci_scratch/ci/scratch/qa-assets/fuzz_seed_corpus/asmap_direct')]INFO: Running with entropic power schedule (0xFF, 100).
INFO: Seed: 3887614080
INFO: Loaded 1 modules   (365146 inline 8-bit counters): 365146 [0x1bf99a0, 0x1c52bfa), 
INFO: Loaded 1 PC tables (365146 PCs): 365146 [0x1c52c00,0x21e51a0), 
INFO:      645 files found in /home/ubuntu/ci_scratch/ci/scratch/qa-assets/fuzz_seed_corpus/asmap_direct
INFO: -max_len is not provided; libFuzzer will not generate inputs larger than 1048576 bytes
INFO: seed corpus: files: 645 min: 1b max: 1048576b total: 15304310b rss: 200Mb
#512	pulse  cov: 238 ft: 1319 corp: 367/51Kb exec/s: 102 rss: 219Mb
#646	INITED cov: 238 ft: 1375 corp: 395/91Kb exec/s: 30 rss: 251Mb
#646	DONE   cov: 238 ft: 1375 corp: 395/91Kb lim: 8193 exec/s: 30 rss: 251Mb
Done 646 runs in 21 second(s)
```

The system installed Valgrind ultimately spends a large amount of time doing:
```bash
rt_sigprocmask(SIG_SETMASK, ~[], ~[ILL TRAP BUS FPE KILL SEGV STOP SYS], 8) = 0
rt_sigtimedwait(~[], 0x1004abce30, {tv_sec=0, tv_nsec=0}, 8) = -1 EAGAIN (Resource temporarily unavailable)
rt_sigprocmask(SIG_SETMASK, ~[ILL TRAP BUS FPE KILL SEGV STOP SYS], NULL, 8) = 0
```

This seems to get progressively worse. i.e a continually increasing time spent doing
this between subsequent fuzz inputs. The built-from-source binary also does this,
but not to the same extent.
