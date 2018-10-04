# __Panic" Instructions / Help__

## __Program `xyz` has crashed/hung and it's affecting the data taking.__

  MIDAS is supposed to have a built in "watchdog" system for removing any crashed programs from the experiment so that they cannot influence the data taking. However, if a program has registered start or stop transitions and has become hung but not fully crashed, it can block the stopping or starting of runs. If this is the case, you will want to kill the offending program manually. Open a secure shell to the host machine running the program and type:
```
$ ps aux | grep program_name
```

  This will display process information of all running processes involving `program_name` in the following
  format:
```
USER              PID  %CPU %MEM      VSZ    RSS   TT  STAT STARTED      TIME COMMAND
```

  To kill the offending program, type `kill xxxxxx`, where `xxxxxx` is the process ID. If this doesn't work, try again with `kill -9 xxxxxx`. Once you have killed the program, the data taking should work again; however, you may want to do a stop run/start run cycle to reset things.

## __MIDAS got hung up when I tried to start or stop a run.__

  Open a terminal window, ssh into smaug (`ssh dragon@smaug` or just `ssh smaug` if working on daenerys) and then run the command `odbedit`. Once in this program, issue the command `stop now -v` (or `start now -v`). The this may solve the problem on it's own, but if not you should see from the output where the transition is getting hung up. If so, kill the offending program (see 1. above) and try again to start/stop the run. Then try re-starting the program you killed, and then try starting a new run to see if the problem is fixed. If the problem persists, more drastic measures may be necessary ([see below](#__Something isn't working and I have no idea what's going on!__).

## __Program `xyz` won't stay open.__

  If a program refuses to start from the MIDAS programs page, click on the link on the left side of the page and look at the "Start command" section to see what the command is to start the program. Then try running this in a shell, replacing any `-D` flags (the "become a daemon" flag) with `-d` (the debug flag) or `-v` (the verbose output flag), depending on the program. This may not get the program to start, but at least the extra terminal output can point you in the right direction towards figuring out the problem.

## __I tried to submit an elog entry and it got hung up.__

  We have noticed that occasionally certain attachments can cause an elog entry to become hung. If this happens to you, first try to copy the entry text to the clipboard or another program so that you do not have to re-type everything. Then close the browser window/tab in which you have tried to submit the elog entry. This should fix the hang-up, but your entry may be missing. If so, re-submit without the attachments. Then try to edit the entry and if the attachment still refuses to upload, you can try breaking it into multiple smaller files (by area) as this seems to fix things. As a last resoort, you can just reference the location of the attachment on `daenerys` and interested readers can go find it manually.

## __The MIDAS status page is spewing errors and I have no idea what they mean.__

  As long as the errors are not coming from one of the frontends (fe_head, fe_tail) then it is likely something non-cricual (probably a bug in one of the analysis codes). In this case, it is not worth stopping data-taking to investigate. If the errors are swamping the MDIAS message system, close the offending client until someone who can debug the problem is around.

  If you are getting the occational error from one of the frontends about "V792 trigger problem", then this is a known, albeit not fully understood issue. You can continue to safely take data if this is the case.

## __The MIDAS status page won't load.__

  There seems to be an issue with the mhttpd program that causes the MIDAS page to get hung up. This doesn't seem to affect the rest of the DAQ, but it does make operation difficult. If this happens, try the following:


  1. Open a terminal and ssh as dragon@smaug.triumf.ca (no password necessary from `daenerys`).
  2. Kill the mhttpd server by typing the following:
```
$ pgrep mhttpd | xargs kill
```
  3. Type the following to see if the process was killed:
```
$ ps ux | grep mhttpd
```
  4. If not, try step 2 again but with `kill -9`:
```
$ pgrep mhttpd | xargs kill -9
```
  5. Check again for mhttpd. Assuming the `kill -9` worked, run the following command:
```
$ mhttpd --https 8443 --http 8081 -D
```


  This should have fixed the problem, so try refreshing the MIDAS status page to see if it's working again.


## __How do I recover from a corrupted ODB?__

  1. Stop the front-ends, mlogger, mhttpd, etc. (this can be accomplished by running `kill_daq.sh`).
  2. Remove the shared memory associated to ODB buffer.
    - Find the shared memory segment
```
$ ipcs -m
```
    - Then remove the shared memory process
```
$ ipcrm shm <shmid>
```
      where `shmid` is the id of the shared memory process you wish to delete
  3. Move the old ODB files. These will be in the location given by Exptab, e.g. ~/online
```
$ cd online
$ mv .ODB.SHM .ODB.SHM.BAD
```
  4. Restart ODB with larger size using odbedit e.g.
```
$ odbedit -s 100000000
```
  5. Reload the last saved ODB dump you have (see save and reload the ODB). These dumps are stored in the current data logging directory (`$DH`) and have the form `runxxxx.xml`. So from odbedit you do something like
```
odbedit> cd /
odbedit> load /path/to/$DH/runxxxx.xml
```

---

```diff
--Warning:
```
  Take care to load ODB dumps only when in the ODB root directory (i.e. - `/`), as loading a file loads the keys present in the file *at the current path* (i.e. - loading `runxxxx.xml` when in the directory `/Equipment` loads the entire ODB again starting at the `/Equipment` directory).

---

  Your ODB should now be fixed, and you can restart the DAQ using the `start_daq.sh` script.

---

``` diff
++Note:
```

  It is sometimes necessary to also remove the other shared memory buffers in `~/online` (e.g. - `.SYSMSG.SHM`, `.MSG.SHM`, etc.).

---
  
``` diff
--Attention:
```

  If the above steps do not solve the problem, it may be necessary to reboot `smaug` and start at step 3.

---

## __Something isn't working and I have no idea what's going on!__

  For DAQ problems where the source is unknown, the standard procedure is to do the following:

  1. Kill and re-start all programs by logging into `smaug` and typing the command `start_daq.sh`. As a second step, double check that none of the frontend programs (fe_head, fe_tail, fewiener, feepics) are running more than one instance: log into the appropriate host (this will be noted from the [link on the MIDAS programs page](https://smaug.triumf.ca:8443/?cmd=Programs)) and do `ps ux | grep program_name`; there should be only one instance of the program running. If there is more than one, kill all of them, then re-start the program from the MIDAS programs page or directly from a terminal.
  
  2. Start up any other clients that aren't automatically started by the `start_daq.sh` command (anaDragon, rootbeer, dragonscaler, feepics) and then try to resume data taking. If everything is looking okay, then you have probably fixed things.
  
  3. If Steps 1 and 2 haven't solved the problem, log into smaug and issue the command `kill_daq.sh`; then go downstairs and reboot the VME crates. There are two crates, located near each of the electronics racks. They are the relatively small, white-colored VME crates with horizontally-oriented modules. To reboot a crate, flip off the power switch, wait ~20 seconds and then switch it back on again. Verify that the crate powers up correctly and that the status lights on the front of all of the modules are green.
     
  4. After rebooting the crates, go back to smaug and type `start_daq.sh`, then start over again at step 2.

  If you are able to get things back to a working state, continue running but make a note of what happened and any details that you think might be relevant in case this might be useful in later figuring out what went wrong and potentially fixing the problem.

## __There is a problem with the DAQ that is not addressed here.__

  For MIDAS / DAQ problems not addressed above (or remedied by any of the above actions), please consult the [MIDAS elog](https://midas.triumf.ca/elog/Midas/) the [MIDAS faq page](https://midas.triumf.ca/MidasWiki/index.php/FAQ),the [MIDAS documentation](https://midas.triumf.ca/MidasWiki/index.php/Main_Page#Documentation), the [MIDAS wiki](https://midas.triumf.ca/MidasWiki/index.php/Main_Page), or a DRAGON expert.

# __Installation Problems__

## __Compilation of `libDragon.so` and/or `mid2root` fails on Ubuntu.__

  I have found Ubuntu to be very picky about compiling the DRAGON analyzer package, and have tried to avoid difficulties by setting appropriate compile time environment variables in the `configure` script and Makefile. Installation on Ubuntu does require dependencies beoynd those mentiond in [the installation instructions](/analyzer/wiki/installation)", namely, it requires that `clang` and `clang++` be installed (`sudo apt-get install clang clang++`), and typically it is necessary to set the macro `_GLIBCXX_USE_CXX11_ABI=0` (switched off by default in the `configure` script). However, I have found that this switch causes compilation failure from time to time, so there is an option to switch it on in the `configure` script; try reconfiguring and recompiling as follows:
```
$ make clean
$ ./configure --use-cxx11-abi
$ make
```

## __Compilation of `libDragon.so` and/or `mid2root` fails on Mac OS X High Sierra (OS X 10.13.x).__

  If compilation fails on High Sierra, it is likely that you have installed (or your `$ROOTSYS` points to) ROOT `5.34`, which [doesn't build properly on OS X High Sierra](https://root-forum.cern.ch/t/root-5-34-with-high-sierra/27158). If you recently updated to High Sierra, you will need to rebuild ROOT `5.34/xx`. If you have already done so, starting ROOT and attempting to load `LibRIO.so` will result in (something like) the following output:

```
root[0] gSystem->Load("LibRIO.so");
dlopen error: dlopen(/Users/couet/git/root534-cocoa-bin/lib/LibRIO.so, 9): can't resolve symbol __ZTVN10__cxxabiv117__class_type_infoE in /Users/couet/git/root534-cocoa-bin/lib/libThread.so because dependent dylib #3 could not be loaded in /Users/couet/git/root534-cocoa-bin/lib/libThread.so
Load Error: Failed to load Dynamic link library /Users/couet/git/root534-cocoa-bin/lib/LibRIO.so
```
  
  To get a working ROOT `5.34`, rebuild ROOT `5.34` (if you have not done so already), copy the directories missing from `$ROOTSYS/main/` (in particular `$ROOTSYS/main/src`) from the repository then recompile using `root-config` as follows:
  
```
$ cd /path/to/root534
$ source bin/thisroot.sh                                                               # source ROOT environment
$ rsync -avz /path/to/root.git/main/ ./main/                                           # check out v5-34-xx from the git repo before issuing this command 
$ cd main/src
$ clang++ `root-config --cflags` -o ../../bin/root rmain.cxx `root-config --glibs`     # or g++
$ root
root[0] gSystem->Load("LibRIO.so");
```

## __Do I need to install MIDAS in order to use the DRAGON analyzer?__

  MIDAS is *not* required to install the DRAGON analyzer package. If you are a user and wish to install the DRAGON analyzer package for the purposes of offline data analysis, installation of MIDAS is __not__ recommended.

## __I had to compile ROOT with `cmake`; do I also have to compile the DRAGON analyzer with `cmake`?__

  No, the DRAGON analyzer package is currently compiled using a `bash` configure script and `gnu make`. The package is not currently set up for compilation with `cmake` (but it might be worthwhile to investigate the possibility of setting up compilation with `cmake` if anyone is looking for a side project...).

## __My problem is not addressed here.__

  If your problem is not addressed above, [contact me](mailto:dconnolly@triumf.ca) for futher assistance.
  Please also include any relevant troubleshooting information, such as the compiler output, platform, which compiler was
  used, ROOT version, etc.

# __Frontend and Hardware__

## __How to I add or remove a detector signal from the trigger?__

  For both the head and tail, the trigger is an `OR` of IO32 ECL inputs 0-7. Each of these inputs is masked by an IO32 register that is controlled through ODB variables. So, to add or remove a detector signal from the trigger, first identify which ECL channel the signal in question is plugged into. It should be one of the first eight channels on the LeCroy NIM->ECL converter labeled with "trigger + scalers" or "IO32".

  Once you have identified the channel in question, edit the corresponding ODB variable `/Equipment/xxxxVME/Settings/IO32/ChannelEnable[n]`, where `xxxx` is either `Head` or `Tail`, and `n` is the channel number in question (note that the NIM->ECL converters are labeled starting with 1, whereas the ODB variables start at 0). Setting the `ChannelEnable[n]` variable to `y` adds the signal to the trigger, while `n` removes it.

  You can look here: /analyzer/docs/html/frontend/html/hardware.html#ecl_in for a listing of which detectors are plugged into which ECL channels.

## __What are all those signals going into and out of the IO32?__

  See the following page:	/analyzer/docs/html/frontend/html/hardware.html for an overview of the signals originating from and going into the IO32.

## __What about the scalers?__

  Scaler inputs are discussed at /analyzer/docs/html/frontend/html/hardware.html#scalers

## __What about the EPICS scalers?__

  See the [EPICS scaler page](docs/html/frontend/html/hardware.html#epicsscalers) for a listing of EPICS scaler inputs.
  
---

``` diff
--Attention:
```
  The VME scaler module that readout the EPICS scalers failed in the Summer of 2017 (although there are still copies of the channels mentioned in the above link routed to this module from the NIM->ECL level converter). We now deliver EPICS scaler information to ISAC operators via EPICS virtual process variables through the EPICS virtual scaler frontend. The io32 now writes scaler information for the Head, Tail and Aux scalers to the new ODB equipment banks vHeadScaler, vTailScaler and vAuxScaler. The frontend (fevScaler.c) handles writing of ODB variables from these equipment banks to the EPICS virtual process channels. The EPICS virtual process variables can be found by clicking on the DRAGON SCLR Vars under the Diagnositcs menu in EPICS. There are three "banks" of 20 variables (`DRA:SCLR1:VAR1`,..,`DRALSCLR1:VAR20`,`DRA:SCLR2:VAR1`,...,`DRA:SCLR3:VAR1`,...etc.) corresponding to the Head, Tail and Aux scalers (respectively) and they can be striptooled by connecting to `DRA:SCLRi:VARj`, where `i` and `j` are the Equipment bank (1 = Head, 2 = Tail, 3 = Aux) and channel number, respectively.

---

## __What about other signals? Is there a drawing of what goes where?__

  Currently, there isn't a full drawing of all connections, but it is on the to-do list. In the meantime, you can refer to the [hardware](/analyzer/docs/html/frontend/html/hardware.html) and [trigger logic](/analyzer/docs/html/frontend/html/trigger.html) pages to get an idea of what the signals are for and what the various inputs/outputs to the VME modules are. Nearly everything else in the setup, electronics wise, (i.e. all of the NIM modules) is pretty sparse and hopefully can be figured out from cable tracing.

  There is also a [section of the hardware page](/analyzer/docs/html/frontend/html/hardware.html#adc_in) giving the channel mapping for ADCs and TDCs.

## __How to I operate the new frontend?__

  See the operation guide at /analyzer/docs/html/frontend/html/operation.html"

## __What parts of the frontend are configurable? How do I control it?__

  Most of the frontend options (gate widths, delays, etc.) are controlled by ODB variables. See the operation guide [/analyzer/docs/html/frontend/html/operation.html](/analyzer/docs/html/frontend/html/operation.html) for more information.

## __How do I adjust BGO CFD thresholds?__

  From the [MIDAS status page](http://smaug.triumf.ca:8081), first click the `Adjust BGO Thresholds` button. Then change the `Threshold Gamma Energy` variable to the desired threshold energy in keV. Now go back to the status page and click `Apply BGO Thresholds` to make the change. Finally, start a new run for the new threshold values to take effect.

  Note that this only sets the correct keV threshold when the array is gain matched in the standard manner, that is, placing the 6.13 MeV <SUP>13</SUP>C peak at channel 1471 (or 6.13 MeV using the standard ADC slopes of 0.00415). For alternative gain matching, the relationship between threshold value and keV will need to be figured out empirically.

## __How do I change the TDC maximum hit option?__

  The TDC maximum hit option is set through the `/Equipment/TailVME/Settings/V1190/MaxHits` variable (replace `TailVME` with `HeadVME` for the BGO side). If this variable is set to a power of two (the result, not the power, i.e. set to `64` not `6`) then the set value will be the TDC Max Hits setting. If it's set to `0xffff`, then the max hits option is disabled and the TDC will record as many events as it can hold in memory. Note that the max hits option is common to a bank of 16 channels, i.e. if max hits is set to 64, then TDC channels 0 - 15 can only hold 64 events, 16-31 can only hold 64 events, etc. Also note that an event means a leading or a trailing edge pulse, so one leading+trailing signal will count as two events. For this reason, it is suggested to leave the MaxHits variable set to `0xffff` to avoid corwding out a needed signal with unneeded ones.

## __How can I set ADC thresholds, or suppress readout of no-data events?__

  The CAEN V785/V792 ADCs have the option to suppress the readout of data that falls below some set value. Typically, this might be desired to suppress the read out of QDC pedestals or ADC "zero value" peaks to save disk space and reduce read out time.

  These settings are controlled in the ODB via `/Equipment/TailVME/Settings/V785/` (sub `HeadVME` for `TailVME` and `V792` for `V785` for the BGO side). The `EnableThreshold` variable either turns on or off zero suppression. Setting this variable to `n` means that all data are read out, regardless of the size of the converted value. Setting it to yes means that only data above threshold values are read out. If there are multiple ADCS, the `EnableThreshold` is an array, with each index being common to one of the ADCs (currently in the tail, [0] means the non-DSSSD ADC and [1] the DSSSD ADC).

  The threshold values are controlled by the `Threshold` array, with each index corresponding to the same channel in the ADC, or in the case of multiple ADCs, the array index corresponds to the channel plus an offset. For example, currently the tail setup has two ADCs, so indices 0-31 control channels 0-31 in `ADC0` (which is the non-DSSSD ADC), and indices 32-63 control channels 0-31 in `ADC1` (the DSSSD ADC). The threshold values should be set to a multiple of 16, if not they will be rounded down to the closest multiple of 16.

## __My question is not addressed here.__

  For questions on DRAGON's frontend not addressed above, please consult the DRAGON Frontend Documentation or a DRAGON expert.

# __Online Analysis__

## __How do I change a variable (ADC pedestal, slope, channel mapping, etc.)?__

  Update the corresponding value in the MIDAS ODB, and start a new run (or re-start the analysis program). The online analysis routines automatically sync all variables with the ODB at the beginning of each run, or at program start (this is true both for ROOTBEER and anaDragon/ROODY).

## __Where can I find the ODB variable corresponding to `xxx`?__

  All analysis-related ODB variables are contained under the `/dragon` tree. From here, they are subdivided by detector element, i.e. variables related to the BGO array are under `/bgo/`, etc. From there, follow the remaining subtrees until you find what you want. Hopefully this is intuitive enough to figure out on your own; if it is not, let me (G.C.) know, and I will update this section.

## __How do I add a new histogram to the analyzer?__

  If you are using ROOTBEER, use the histogram creation GUI. If you think your histogram is something that should normally be available for viewing, use the "Save" button on the histogram GUI to save it to a config file. For more information, see http://trshare.triumf.ca/~gchristian/rootbeer/doc/html/gui.html .

  If you are using rootana + ROODY, see the [for users](analyzer/wiki/index#rootana) section of this manual.

## __Where did the scaler histograms go?__

  Scaler histograms are now included in ROOTBEER.

## __How do I use ROOTBEER?__

  To start the program, ssh into smaug and run `rbdragon`. Click the orange `Load` button on the `rootbeer histograms` window to select a histogram definitions file (default is `dragon_hists.C`). This should load a standard set of histograms which you can tweak or add to as needed using the GUI or command line. If you make major changes, be sure to save them (in a different file) using the blue `Save` button on the `rootbeer histograms` page.

  To connect to online MIDAS data, click `Attach Online` on the `rootbeer` window. To manipulate canvases/refresh rate, use the `Canvas` section of the `rootbeer` window. Note that by double clicking on a canvas (or pad) _outside_ the histogram region, you can make that canvas (pad) take focus. You can draw a histogram in the current canvas (pad) by cicking the blue `Draw` button on the `rootbeer histograms` window when the desired histogram is selected in the window above it (or double clicking the desired histogram should have the same effect). Note that histograms are drawn using the `Draw option:` box contents as the `opt` argument, i.e. filling the `Draw option:` box with `COLZ`draws a 2d histogram in color, etc.

  All histograms are automatically cleared (zeroed) at the beginning of a new run, or you can use the `Zero All` and `Zero Current` buttons to manually zero those histograms which are currently drawn in a canvas. If you want to zero *every* histogram manually, type `rb::hist::ClearAll()` in the command window.

  This is just a bare-bones basic introduction; there is more info avaialable at http://trshare.triumf.ca/~gchristian/rootbeer/doc/html/index.html (though some of this may currently be out of date).

## __Can I change whether or not online histograms are zeroed at run start?__

  Yes. The easiest way is to modify the ODB variable ["/dragon/rootbeer/AutoZero"](http://smaug.triumf.ca:8081/dragon/rootbeer). This is an integer which should take one of three values specifying different auto zero conditions:
        - `0` : No auto zeroing (all histograms are untouched at run start).
        - `1` : Scaler histograms are zeroed at run start; all others untouched.
        - `2` : Complete auto zeroing (all histograms zeroed at run start).

# __Offline Analysis__

## __How do I obtain the software package to do offline analysis using ROOT?__

  See the installation instructions [here](index.html#installation).

## __I am using `ROOT6` and recieve the following warning when working with dragon root files created with ROOT5:__

  ```
  Warning in <TStreamerInfo::BuildOld>: Cannot convert dragon::Head::trf from type: dragon::TdcChannel<MAX_RF_HITS> to type: dragon::TdcChannel<5>, skip element
  
  ```

  As far as I can tell, this is functionally inconsequential, but if the warning bothers you, entering the following before loading the root file should solve the issue:
```c++
ROOT::AddClassAlternate("dragon::TdcChannel<5>","dragon::TdcChannel<MAX_RF_HITS>")
```

## __How do I convert MIDAS files into ROOT files?__

  Run the `mid2root` utility that is distributed as part of this software package (it should be located in the `/bin` directory). For information on running the programs, first do `mid2root --help`.

  Note that during an experiment, the lazylogger utility automatically converts every saved MIDAS file into ROOT format. The output files are located in `<data dir>/rootfiles/`, where `<data dir>` is the ODB `/Logger/Data dir` setting at the time of the run. Note that variables used in these conversions are reflect the state of the ODB when the run was started.

## __How can I re-analyze a MIDAS file using different variables?__

  The `mid2root` program allows users to specify an XML file containing the variables to be used for analysis and unpacking into ROOT trees. This is done by using the `-v` flag to specify the desired XML file. The supplied argument must be a valid XML file in the same format as those generated by MIDAS. Since this format is quite hard to generate by hand, it is suggested to first copy one of the `run****.xml` files from the logger data directory and then edit this to update variables as desired.

## __What if I just want to calibrate the DSSSD (or BGO, etc.)? Do I have to re-analyze every MIDAS file, or is there a better way?__

  Re-analyzing a whole set of MIDAS files just to calibrate one or two detectors is indeed inefficient. However, it is possible to do the calibration by looping through the events in the _ROOT_ file, applying the calibration, and saving the results in an external file. Then using the [`TTre::AddFriend`](http://root.cern.ch/root/html/TTree.html#TTree:AddFriend) utility, you can view the results on an event-by-event basis, just as if they were members of the original tree.

  For the DSSSDs, a "selector" class has already been written to facilitate re-calibrating the array and saving the results in separate files, run-by-run. See the `dragon::DsssdCalibrate` class documentation and the file [`DsssdCalibration.C`](_dsssd_calibration_8_c.html) in the examples directory.

## __Now that I've converted MIDAS files into ROOT format, how can I see the data?__

  There are a few options here. One is to create a [`TChain`](http://root.cern.ch/root/html/TChain.html) linking the events in all of your ROOT files together, and then start viewing data using the [`Draw()`](http://root.cern.ch/root/html/TTree.html#TTree:Draw@2) command. For analyses with a relatively small number of events and minimal data processing requirements, this is often the fastest and easiest solution.

  If your analysis requirements include handling a large number of events (typically > 1 million or so will noticeably slow down the `Draw()` command), or performing non-trivial transformations on the data, there are some classes defined in [`RootAnalysis.hxx`](_root_analysis_8hxx.html) and [`Selectors.hxx`](_selectors_8hxx.html) that can help you out. One is a class to filter a chain of files into a smaller number of events based on some cut condition and save the result in a ROOT file. Often, one pass through the data with a rough cut can reduce the dataset to a managable size for further analysis. The other classes available are a set of TSelector classes specific to DRAGON data. By deriving from these, you can code your analysis routines in `C++`. See the documentation on [`RootAnalysis.hxx`](_root_analysis_8hxx.html), [`Selector.hxx`](_selectors_8hxx.html), or the files in the examples directory for further information.

## __What if I want to get the value of some parameter for a specific event or events, for example to use elsewhere in a macro?__


  There are a few ways to accomplish this:

  1. Use `TTree::GetEntry()`

```c++
dragon::Tail* ptail = new dragon::Tail(); // create an instance of the tail class to store event-by-event data
t3->SetBranchAddress("tail", &ptail);     // 't3' now associates the "tail" branch with 'ptail'
t3->GetEntry(0); // 'ptail' now holds all the data from event 0
double mcpTac = ptail->mcp.tac;

//// use mcpTac however you need to //

t3->ResetBranchAddresses(); // free 'ptail' from it's association with "t3"
delete ptail; // release the memory allocated to ptail
```

  Typically this method is best if you need to loop over many entries and use a large number of parameters in your loop code. Otherwise, the syntax is quite cumbersome, and it is rather poor performance-wise since you have to read _all_ of the data from every event, rather than just the branches you want.
  
  2. Use `TLeaf::GetValue()`

```c++
t3->GetEntry(0); // load the 0th entry in the tree
double mcpTac = t3->GetLeaf("mcp.tac")->GetValue(); // read the value at "mcp.tac"
```
  Note that for arrays, one must specify the array index as the `GetValue()` argument, _not_ in the `GetLeaf()` string argument:

```c++
t3->GetEntry(0);
double dsssd15 = t3->GetLeaf("dsssd.ecal")->GetValue(15);
```
  This method is nice if you just want to get the values of a few parameters for a few events.
  
  3. Use `TTree::Draw()` -

```c++
t3->Draw("mcp.tac", "", "goff"); // no graphics output, but the drawn data are saved event-by-event in arrays
double mcpTac = t3->GetV1()[0];  // mcpTac is now set to the 'mcp.tac' value from event 0
```
  You can also Draw and extract two or more (up to four) parameters at once.

```c++
t3->Draw("dsssd.ecal[15]:mcp.tac", "", "goff");
double dsssd15 = t3->GetV1()[0];  // dsssd15 is now set to the 'dsssd.ecal[15]' value from event 0
double mcpTac  = t3->GetV2()[0];  // mcpTac is now set to the 'mcp.tac' value from event 0
```
  This method has the advantage that you can specify a gate in the Draw() command, thus allowing you to loop over only the events satisfying the cut condition. It is also very efficient and thus good for looping over all or part of a tree and extracting a few parameters event-by-event.

---

``` diff
--Warning:
```
  This method does have one potential pitfall: The arrays returned by `GetV1()`, etc. have a default maximum size of 10<sup>6</sup> events. If your Tree has more entries than this, then using the brackets [] to get entries after the 10<sup>6</sup>th entry will be undefined. To rectify this, use `TTree::SetEstimate()` -

```c++
Long64_t nevts = t3->Draw("mcp.tac", "", "goff"); // (returns, say, 5000000)

//// - BAD - //
for (Long64_t event = 0; event < nevts; ++event) {
  std::cout << "Event " << event << ": mcp.tac = " << t3->GetV1()[event] << "\n"; // BAD!!! for event > 999999, the result is undefined
}

//// - GOOD - //
t3->SetEstimate(nevts);
for (Long64_t event = 0; event < nevts; ++event) {
  std::cout << "Event " << event << ": mcp.tac = " << t3->GetV1()[event] << "\n"; // Okay, GetV1() now valied up to index [nevts-1]
}
```

---

## __I'm really new to C++/ROOT and have no idea how to even get started looking at data. Any advice?__

ROOT does indeed have a steep learning curve, and most of the documentation on their website assumes a certain level of familiarity with C++.

<!--  For the true beginners, Greg Christian wrote a ROOT guide a few years ago for a different group that can hopefully be useful. Some of it is specific to that group, but the basics are the same. It is available online here: <a href="http://www.cord.edu/dept/physics/mona/manuals/root_guide.pdf"</a> -->

Useful resources include the [ROOT guide](http://root.cern.ch/drupal/content/users-guide) (though this often is not the best for beginners), the [ROOT Primer](https://root.cern.ch/guides/primer), and the [ROOT Tutorials](https://root.cern.ch/doc/master/group__Tutorials.html) (the code for which are also included with your installation of ROOT at `$ROOTSYS/tutorials`). Often, a good reference is the [ROOT reference documentation](https://root.cern.ch/doc/v610/index.html) (e.g. - [the `TTree` class documentation](http://root.cern.ch/root/html/TTree.html)).

A useful resource for learning C++ is [cplusplus.com](http://www.cplusplus.com/), and the [tutorials](http://www.cplusplus.com/doc/tutorial/) therein.

## __I can never remember all of the parameter names in the ROOT trees. Is there a way to find this out easily?__

  Use the `TTree::StartViewer()` command, e.g. `t3->StartViewer()` to see a complete graphical listing of all
  avaiable parameters in a given tree. Alternatively, you can use `TTree::Print`, e.g. `t3->Print()` to see a
  textual listing of available parameters.

## __How do you make plots of energy vs. channel in offline analysis from Trees. For example: DSSSD energy vs channel?__

  Take advantage of `TTree::Draw()` and its features for parsing arrays. To draw DSSSD energy vs. channel just
  do the following:
  
```c++
t3->Draw("Iteration$:dsssd.ecal[]>>hst(400,0,4000,32,0,32)", "", "colz");
```

  For more information on the options available using `TTree::Draw()`, see the [class method documentation](http://root.cern.ch/root/html/TTree.html#TTree:Draw@2).

# __Generic (Online/Offline) Analysis__

## __What do all the parameter names mean? How can I find the parameter name corresponding to xyz signal?__

  Short answer: see the [Parameter Names](/analyzer/wiki/params) page for a description of the most commonly used `TTree` parameters

  Long answer: Each detector element in DRAGON is represented by a C++ class in the analyzer. Each detector class contains data members which correspond to experimental parameters recorded by the corresponding detector. By arranging the various detector classes in a hierarchy, we create a tree-like structure describing experiment parameters. For example, the DSSSD parameters are arranged as follows:
  
```c++
class Dsssd {
public:
///// Calibrated energy signals
  double ecal[MAX_CHANNELS]; //#
  ///// Highest energy signal in the front strips (0 - 15)
  double efront;      //#
  ///// Highest energy signal in the back strips (16 - 31)
  double eback;       //#
  ///// Which strip was hit in the front strips
  uint32_t hit_front; //#
  ///// Which strip was hit in the back strips
  uint32_t hit_back;  //#
  ///// Calibrated time signal from the front strips
  double tfront;      //#
      ///// Calibrated time signal from the back strips
  double tback;       //#

  //// etc //
};

class Tail {
public:
    Dsssd dsssd;
    //// etc //
};
```

  This sets the DSSSD as a sub-element of Tail, with each of the DSSSD data members being sub-elements of DSSSD. Thus the full hierarchy for, say, the energy of channel 16 is `tail.dsssd.ecal[16]`. When using ROOT, the class hierarchy is parsed into a `TTree` branch structure, meaning that the parameter name for DSSSD energy, channel 16 is exactly as noted.

## __What am I looking at when I look at the TDC (tcal) or tof parameters?__

  This can get a little confusing because of the way the TDCs operate. Briefly, in order to cover the range of times we need, we have to send the TDC a "stop-like" signal that comes _after_ any measurement pulse. This "stop" is generated by the IO32 with a programmable delay relative to the trigger. Making the delay introduces a jitter to the stop signal, so any measurement value coming from the TDC will also contain the same jitter.

  The jitter mentioned above is common to every measurement channel, so it can be removed by simply subtracting channels from each other in software. However, this is _not_ done in the default anaylzer for the `tcal` parameters - what you are looking at when you view, say, `bgo.tcal[0]` contains jitter. The `tcal` parameters have, however, been calibrated into nanoseconds by multiplying by a slope of 0.1 (the nominal resolution of the TDC is 100 ps / channel the way we operate it). If you want to view calibrated times without jitter, the easiest thing to do is to subtract off the trigger time (`tcal0`) yourself, i.e.
  
```c++
t1->Draw("bgo.tcal[0] - tcal0")
```
  
  The time-of-flight parameters (e.g. `coinc.xtofh`, `hi.tof.mcp_dsssd`, etc.) _do_ have jitter subtracted since they are calculated by subtracting two `tcal` values from each other.

## __How is the coincidence matching done?__

  See [this section](index.html#coincmatch) for an overview of the algorithm used for coincidence matching. The same algorithm is used whether in online or offline mode. Also, see the documentation on `tstamp::Queue` for some more details about the implementation of the matching algorithm.

## __Are EPICS data logged anywhere?__

  Yes, every time the EPICS frontend polls a channel for data, it sends the results to the `SYSTEM` buffer. From there, they go into both the online analysis pipeline and into the saved `run****.mid` file if data recording is turned on. The analyzer upacks EPICS events into a ROOT tree (`t20`) with two parameters (in addition to a MIDAS event header): channel number [`ch`] and value [`val`]. So to see, for intance, the recorded values of EPICS channel 0 versus time, plot the following:
  
```c++
t20->Draw("val:header.fTimeStamp", "ch == 0");
```
  
## __What is the channel number for EPICS device *x*?__

  You can find the full list of the EPICS channels and their EPICS channel names at https://smaug.triumf.ca:8443/Equipment/Epics/Settings (requires a TRIUMF IP address) or [here](/analyzer/wiki/epics_chans). Some commonnly accessed channels are:

  Channel | Epics Device Name      | Description                                                               |
  --------|------------------------|---------------------------------------------------------------------------
  [0]     | `DRA:CMGC:RDVAC`       | Gas target pressure [Torr]
  [1]     | `DRA:TCM1:GT:RDTEMP`   | Gas target temperature [&deg;C]
  [2]     | `CCS2ISAC:BL2ACURRENT` | BL2A Proton current [&mu;A]
  [23]    | `DRA:MD1:RDNMR`        | MD1 NMR probe read back [Gauss]
  [80]    | `DRA:XSLITM:SCALECUR1` | Left horizontal mass slit current read back [A]
  
---

``` diff
--Attnention:
```
The above channels are the current EPICS channel values (circa Jan. 2016 - present). If you are working with data from another time period, consult the `.xml` files in your data set to find the desired EPICS channel by searching for the "EPICS_NAMES" key array in the Epics ODB tree.

---

<!-- Local Variables: -->
<!-- mode: gfm -->
<!-- End: -->
