---
title: "For Developers"
keywords: developers
sidebar: mydoc_sidebar
permalink: develop.html
---

## __For Developers__

This section just gives a brief overview behind the package's design and some suggestions for future maintainers/developers. For detailed documentation, see the related Doxygen documentation linked from the top of this page.

### __Design__

In writing this code, I have tried to adhere to a few design principles. I will give a brief listing here in hopes that future developers (or users) of the code can better understand what is going on and the reasoning behind it. In the future (barring a major re-write), I think it will be beneficial to follow a similar design pattern for modifications and updates to the code, if for no other reason than to keep a sense of uniformity which will hopefully facilitate understanding of the code.

- __Separate the handling of "raw" data from that of abstract experiment parameters.__
  
  By this I mean separating the "raw" tasks of reading data directly from VME modules from that of organizing the data into parameters which represent more abstract experimental quantities. As a simple example, separate the "raw" task of reading the conversion value of QDC channel 27 from the "abstract" task of realizing that this value corresponds to the energy deposited in BGO detector number 27. The hope is that by de-coupling the two different types of operations, we will gain greater flexibility and re-usability of the code.
  
  To accomplish this, there are separate "vme" classes (under the [vme namespace](namespacevme.html)) which handle the unpacking of raw vme data. Each vme class contains data fields to store its respective measurements. For example, see the documentation of `vme::V792`, which represents a CAEN V792 QDC module.

- __Store abstract experiment parameters in classes which logically correspond to the separate pieces of a DRAGON experiment.__

  The basic idea is that we want to take the important data captured during a DRAGON event and break it down into a tree-like structure. In general, each "branch" of the tree corresponds to a different detector within the DRAGON system, and the various "leaves" correspond to quantities measured by that detector. For example, [see the `dragon::Coinc`](docs/html/analyzer/html/classdragon_1_1_coinc.html) documentation and the "Attribute" links therein.
  
  Taking this approach allows a couple of practical advantages beyond code organization and maintenance. The main one is that we can use ROOT's dictionary facility to create `TTree`s whith branches defined by the structure of our classes. For example we can just do:

  ``` c++
  dragon::Coinc* coinc = new dragon::Coinc();
  TTree tcoinc("tcoinc","Coincidence event");
  tcoinc.Branch("coinc","dragon::Coinc",&coinc);
  ```
  After just a few lines of code, we now have the ability to access any parameter in a DRAGON coincidence event through the 'tcoinc' tree:

  ``` c++
  tcoinc.Draw("coinc.head.bgo.ecal[0]>>hst_e0(400,0,4000)"); // Draws BGO energy, channel 0
  tcoinc.Draw("coinc.tail.mcp.tac>>hst_tac(200,0,2000)"); // Draws MCP TOF
  ```
  In writing the DRAGON classes, I have not used any sort of inheritance, as there is really no compelling reason to do so (in particular, we have no need for dynamic polymorphism). I have, however, tried to maintain a uniform interface for each of the classes. For example, each detector class has the following three methods:
  
  1. `read_data()` - Takes raw data from vme module classes and maps it into the class's internal data structures.
  2. `calculate()` - Performs any necessary calculations on the raw data; for example, this could include things such as pedestal subtraction, channel calibration, or calculation of quantities which depend on multiple signals.
  3. `reset()` - Resets all of the class's internal data to a "default" throwaway value; in most cases this will be equivalent to `dragon::NO_DATA`.
   
  Developing with a common interface, aside from being "neater" allows for some practical advantages, such as using template "helper" functions to bundle routines used for a variety of different classes. For example:

  ``` c++
  template <class T>
  void handle_event(T& detector, const vme::V785 adc[], const vme::V1190& tdc)
  {
    detector.reset();
    detector.read_data(adc, tdc);
    detector.calculate();
  }
    
  void handle_all_events()
  {
    handle_event(mcp, adc, tdc); // mcp is an instance of dragon::Mcp
    handle_event(ic, adc, tdc); // ic is an instance of dragon::IonChamber
    // ... etc ... //
  }
  ```
  
  Another feature that I have adhered to is to allow class data members to be public. Although this goes against canonical object oriented good principles, I think the advantages outweight the disadvantages. This is particularly true if we are working in CINT to analyze data.[^1] For example, with public data, it's trivial to loop over a tree, calculate some new parameter and display it in a histogram. For example, say I want to plot the square of MCP time-of-flight in cases where it is nonzero:

  ``` c++
  // Assume prior existance of a TTree 't' containing heavy-ion data
  dragon::Tail tail;
  void* tailAddr = &tail;
  t->SetBranchAddress("tail", &tailAddr);
  TH1D hst("hst", "MCP Tof **2", 100, 0, 10000);
  for(Long64_t event = 0; event < t->GetEntries(); ++event) {
    t->GetEntry(event);
    if (tail.mcp.tac > 0) {
      hst.Fill(pow(tail.map.tac, 2));
    }
  }
  hst->Draw();
    ```
  
  However, despite the usefulness of public data for scripting use, it is still a good idea to design the _source_ code as if the data were private. This reduces the coupling between the various classes, and as a result making a small change to the structure of one class (or module) is guaranteed not to affect the others. To enforce this, I have defined a macro `PRIVATE` which can be set to be equal to either `private` or `public` in the Makefile and defined all of the class data members under this token.

  ``` c++
  class SomeDetector {
  PRIVATE:
    double some_parameter;
    // etc. //
  };
  ```
  
  When developing the source code, one should make sure it will compile with PRIVATE=private  - to enforce good class design. Then when set up for "production" mode, switch over to PRIVATE=public to allow direct variable access in CINT (and in the python modules, if used).
  
- __Add a layer of abstraction for commonly performed routines.__
  
  For example, to read raw QDC data into the `dragon::Bgo` class, we _could_ do:
  
  ``` c++
  void dragon::Bgo::read_data(const vme::V792& adc, const vme::V1190& tdc)
  {
    for (int i=0; i< MAX_CHANNELS; ++i) {
      ecal[i] = vme.get_data(variables.adc.channel[i]);
    }
  }
  ```
  
  but instead we prefer to do the following, using [`utils::channel_map()`](docs/html/analyzer/html/namespacedragon_1_1utils.html#a14c7c20285926a9a018da79f8c76cd1d) to do the actual mapping.

  ``` c++
  void dragon::Bgo::read_data(const vme::V792& adc, const vme::V1190& tdc)
  {
    utils::channel_map(ecal, MAX_CHANNELS, variables.adc.channel, adc);
  }
  ```
  
  The main motivation for this is to concentrate the majority of the "heavy lifting" used in various calculations into one place. This helps to reduce the occurrence of bugs due to re-writing of similar code and, if bugs are found, to facilitate easier fixes. Most of the "heavy lifting" is done in free functions contained in the `dragon::utils` namespace, or using [standard library algorithms](http://www.cplusplus.com/reference/algorithm/).	

### __Coding Conventions__

Finally, there are a few stylistic issues to address. Much of this is just preference, but it would be nice to keep consistency in future code.

- Make use of namespaces to logically group related classes together. For example, all classes representing vme modules are under the [vme namespace](namespacevme.html), all classes representing dragon detectors or collections of detectors are under the [`dragon`](namespacedragon.html) namespace, all midas-related stuff under the [`midas`](namespacemidas.html) namesapce and so on. Source files belonging to a common namespace are grouped into the same subdirectory within `src/`.

- Class names generally start with a capitol letter and use "CamelCase" when consisting of multiple words. Acronyms are not treated any differently, to avoid confusion; for example `class Dsssd` _not_ `class DSSSD`.

- Class methods and free functions are all "CamelCase" (including acronyms) (`SomeMethod` or `SomeFunction` or `SomeMthd`).

- Brackets: for everything except function definitions, keep the opening bracket on the same line and the trailing bracket on a new line:
  
  ``` c++
  namespace dragon {
    class Bgo {
    public:
      void calculate();
      // ... //
    };
  } // namespace dragon

  void dragon::Bgo::calculate()
  {
    for (int i=0; i < MAX_CHANNELS; ++i ) {
      // ... //
    }
  // ... //
  }
  ```

- In any class that might be parsed into a tree structure using rootcint, class members are all lowercase, should be descriptive yet brief when parsing classes into a tree structure, and should _not_ use any form of Hungarian notation (as it will conflict with the "nicely descriptive" requirement). This has a practical advantage when we let ROOT parse our classes into a `TTree`: the brach names make sense and are logical. I would much rather type:
  
  ``` c++
  t->Draw("head.bgo.ecal[0]");
  ```
  
  than
  
  ``` c++
  t->Draw("gHead.fBgo.fEnergy[0]"); // (following the ROOT / Taligent convention)
  ```
  
  For classes that are not intended to be made `TTree` branches, the ROOT/Taligent hungarian notation can be quite nice, however.
  
- Whenever appropriate, prefer arrays to separate variables. For example, BGO energies are defined as  

  ``` c++
  double ecal[MAX_CHANNELS];
  ```

  instead of

  ``` c++
  double ecal0;
  double ecal1;
  // ... //
  double ecal29;
  ```

  This has a number of practical advantages, such as allowing for loops and std::algorithms to handle the data, and in general it makes life much easier in later analyses as well. It also avoids any issues about whether to start counting at one or zero, since arrays always start from zero.

### __Documentation__

Given the fast turnover rate of developers and the common requirement for "everyday" users (whose level of expertise in the source language ranges from beginner to expert) to understand and modify source code, maintaining quality code documentation is essential for a physics software package to stay maintainable.

As you may have noticed, the present source code has been extensively documented using [Doxygen](http://doxygen.org). I do not claim that the present documentation is perfect by any means, but I hope it is at least a large step in the right direction. If you happen to extend or modify the code base, _please carefully document anything that you do._ If you are not familiar with Doxygen, it is quite intuitive and quick to learn - just follow the example of existing code and/or refer to the [Doxygen manual](http://www.stack.nl/~dimitri/doxygen/manual/index.html).

---

[^1]: Actually, because the dictionary file generated by rootcint uses the  `#define private public` trick to gain access to your class's internals, if you _don't_ make all of your class data public you actually end up generating code that is not within the C++ standard and, in theory at least, can lead to undefined behavior! See, for example, [__this link__](http://www.gotw.ca/gotw/076.htm) for more on this subject. So, if you want to be as standards compliant as possible, you will have to make all of your class members public.


<!-- Local Variables: -->
<!-- mode: gfm -->
<!-- End: -->
