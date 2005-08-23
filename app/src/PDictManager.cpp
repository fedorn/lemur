/*==========================================================================
 * Copyright (c) 2003-2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/
/// Probabilistic Dictionary interaction

/*! \page PDict  Probabilistic Dictionary Maintenance
  Provides a command line interface for interacting with probabilistic
  dictionaries (PDict objects). It supports the following activities:
  <ul>
  <li> Create a new dictionary from one or more token delimited input files.
  The input file must contain 4 columns. The columns are:<br>
  sourceterm;type;targetterm;probability;<br>
  where type is an arbitrary symbol, such as a part of speech tag.
  <li> Merge two existing dictionaries into a new dictionary.
  <li> Subtract an existing dictionary (prune) from a dictionary.
  <li> Normalize probabilities for all entries
  <li> Write the dictionary to a token delimited output file.
  <li> Interactively look up individual entries, which are displayed 
       (unfiltered) to the console device.
  </ul>

<p>These activities are supported in interactive mode and in batch mode via 
  command line parameters.

<p>  For interactive mode the commands are:
  <ul>
    <li> help -- command list.
    <li> quit -- exit the program.
    <li> open &lt;dictName&gt; -- open an existing dicitionary.
    <li> close -- close the open dictionary.
    <li> stats -- show dictionary vocabulary statistics.
    <li> dump &lt;outputFile&gt; &lt;delimiter&gt; -- dump the dictionary to outputFile, using delimiter as the separator.
    <li> look &lt;term&gt; -- lookup term in the dictionary.
    <li> normalize -- Normalize probabilities for all entries.
    <li> create &lt;dictName&gt; &lt;inputFile&gt; &lt;delimiter&gt; -- create a new dictionary from an input file.
  The input file must contain 4 columns. The columns are:<br>
  sourceterm;type;targetterm;probability;<br>
  where type is an arbitrary symbol, such as a part of speech tag.
    <li> merge &lt;firstDict&gt; &lt;secondDict&gt; &lt;newName&gt; -- merge firstDict and secondDict into a new dictionary named newName.
    <li> subtract &lt;dictName&gt; -- Remove all entries in dictName from the current dictionary. Destructively modifies the current dictionary.
 </ul>

<p>For batch mode the command line parameters are:
<ul>
    <li> -help -- usage message.
    <li> -dump &lt;inputDict&gt; &lt;outputFile&gt; &lt;delimiter&gt; -- dump the dictionary inputDict to outputFile, using delimiter as the separator.
    <li> -look &lt;inputDict&gt; &lt;term&gt; -- lookup term in the dictionary.
    <li> -normalize &lt;inputDict&gt; -- Normalize probabilities of all entries
    <li> -create &lt;dictName&gt; &lt;inputFile&gt; &lt;delimiter&gt; -- create a new dictionary from an input file. 
  The input file must contain 4 columns. The columns are:<br>
  sourceterm;type;targetterm;probability;<br>
  where type is an arbitrary symbol, such as a part of speech tag.
    <li> -merge &lt;firstDict&gt; &lt;secondDict&gt; &lt;newName&gt; -- merge firstDict and secondDict into a new dictionary named newName
    <li> -subtract firstDict secondDict -- Remove all entries in secondDict from firstDict. Destructively modifies firstDict
</ul>
*/
/*
  author: dmf
 */

// includes TextHandlerManager
#include "PDict.hpp"
using namespace lemur::api;

/// The command interpretor for interacting with a PDict.
class CommandInterp {
  public:
  CommandInterp() : dict(NULL){}
  ~CommandInterp() { delete(dict); }
  // interactive, gets additional parms from cin
  // and calls apropriate method(s)
  bool process(string &cmd) {
    if (cmd == "quit"){
      return false;      
    } else if (cmd == "help") {
      usage();
      return true;
    } else if (cmd == "create") {
      string name;
      cin >> name;
      string infile;
      cin >> infile;
      string delim;
      cin >> delim;
      // need flag for counts domain here.
      create(name, infile, delim);
      return true;
    } else if (cmd == "merge"){
      string firstname, secondname, newname;
      cin >> firstname;
      cin >> secondname;
      cin >> newname;
      merge(firstname, secondname, newname);
      return true;
    } else if (cmd == "open"){
      string name;
      cin >> name;
      open(name);
      return true;
    } else if (cmd == "close"){
      if (dict == NULL) {
        return true;
      }
      close();      
      return true;
    } else if (cmd == "subtract"){
      string name;
      cin >> name;
      if (dict == NULL) {
        cerr << "Open a dictionary first." << endl;
        return true;
      }
      subtract(name);
      return true;
    } else if (cmd == "normalize"){
      if (dict == NULL) {
        cerr << "Open a dictionary first." << endl;
        return true;
      }
      normalize();
      return true;
    } else if (cmd == "stats"){
      if (dict == NULL) {
        cerr << "Open a dictionary first." << endl;
        return true;
      }
      stats();
      return true;
    } else if (cmd == "dump"){
      if (dict == NULL) {
        cerr << "Open a dictionary first." << endl;
        return true;
      }
      string name;
      cin >> name;
      string delim;
      cin >> delim;
      dump(name, delim);
      return true;
    } else if (cmd == "look"){
      string term;
      //      cin >> term;
      cin.ignore(); // munch the space
      getline(cin, term);
      if (dict == NULL) {
        cerr << "Open a dictionary first." << endl;
        return true;
      }
      lookup(term);
      return true;
    } else {
      // unrecognized command
      usage();
      return true;
    }
  }
  // process command line arguments.
  void process(int argc, char *argv[]) {
    // must be at least one.
    string cmd(argv[1]);
    if (cmd == "-create") {
      // need flag for counts domain here.
      // need sanity checks here.
      if (argc != 5) {
        cerr << "-create must specify outname, inname, delimiter" << endl;
        return;
      }
      string name(argv[2]);
      string infile(argv[3]);
      string delim(argv[4]);
      if (!create(name, infile, delim)) {
        cerr << "create " << name << " failed" << endl;
        return;
      }
      dict->close();
      delete(dict);
      dict = NULL;
      return;
    } else if (cmd == "-merge"){
      if (argc != 5) {
        cerr << "-merge must specify firstname, secondname, newname" << endl;
        return;
      }
      string firstname(argv[2]), secondname(argv[3]), newname(argv[4]);
      merge(firstname, secondname, newname);
      return ;
    } else if (cmd == "-subtract"){
      if (argc != 4) {
        cerr << "-subtract must specify dictionary, dictToSubtract" << endl;
        return;
      }
      string dictName(argv[2]);
      string name(argv[3]);
      dict = new lemur::dictionary::PDict();
      if (! dict->open(dictName)) {
        cerr << "Unable to open " << dictName << endl;
        delete(dict);
        dict = NULL;
        return; 
      }
      subtract(name);
      dict->close();
      delete(dict);
      dict = NULL;
      return ;
    } else if (cmd == "-normalize"){
      if (argc != 3) {
        cerr << "-normalize must specify dictionary name" << endl;
        return;
      }
      string dictName(argv[2]);
      dict = new lemur::dictionary::PDict();
      if (! dict->open(dictName)) {
        cerr << "Unable to open " << dictName << endl;
        delete(dict);
        dict = NULL;
        return; 
      }
      normalize();
      delete(dict);
      dict = NULL;
      return ;
    } else if (cmd == "-dump"){
      if (argc != 5) {
        cerr << "-dump must specify dictionary name, output name, delimiter" 
             << endl;
        return;
      }
      string dictName(argv[2]);
      string name(argv[3]);
      string delim(argv[4]);
      dict = new lemur::dictionary::PDict();
      if (! dict->open(dictName)) {
        cerr << "Unable to open " << dictName << endl;
        delete(dict);
        dict = NULL;
        return; 
      }
      dump(name, delim);
      dict->close();
      delete(dict);
      dict = NULL;
      return ;
    } else if (cmd == "-look"){
      if (argc < 4) {
        cerr << "-look must specify dictionary, term" << endl;
        return;
      }
      string dictName(argv[2]);
      dict = new lemur::dictionary::PDict();
      if (! dict->open(dictName)) {
        cerr << "Unable to open " << dictName << endl;
        delete(dict);
        dict = NULL;
        return; 
      }
      for (int i = 3; i < argc; i++) {
        string term(argv[i]);
        lookup(term);
        cout << endl;
      }
      dict->close();
      delete(dict);
      dict = NULL;
      return ;
    } else {
      usage(true);
    }
  }
 
  void usage(bool cmdLine = false) {
    // print out usage message
    if (cmdLine) {
    cout << "Available commands" << endl;
    cout << "-help -- this message" << endl;
    cout << "-dump <inputDict> <outputFile> <delimiter>\n"
         << "\t-- dump the dictionary inputDict to outputFile, "
         << "using delimiter\n\t as the separator." << endl;
    cout << "-look <inputDict> <term> -- lookup term "
         << "in the dictionary." << endl;
    cout << "-normalize <inputDict> -- Normalize probabilities of all entries"
         << endl;
    cout << "-create <dictName> <inputFile> <delimiter> -- "
         << "create a new dictionary\n\tfrom an input file." << endl;
    cout << "-merge <firstDict> <secondDict> <newName>\n\t-- " 
         << "merge firstDict and secondDict into a new dictionary "
         << "named newName" << endl;
    cout << "-subtract firstDict secondDict\n\t-- Remove all entries in " 
         << "secondDict from firstDict.\n\tDestructively modifies firstDict" 
         << endl;
    } else {
    cout << "Available commands" << endl;
    cout << "help -- this message" << endl;
    cout << "quit -- exit the program" << endl;
    cout << "open <dictName> -- open an existing dicitionary." << endl;
    cout << "close -- close the open dictionary." << endl;
    cout << "stats -- show dictionary vocabulary statistics." << endl;
    cout << "dump <outputFile> <delimiter> -- dump the dictionary " 
         << "to outputFile,\n\tusing delimiter as the separator." << endl;
    cout << "look <term> -- lookup term in the dictionary." 
         << endl;
    cout << "normalize -- Normalize probabilities for all entries" << endl;
    cout << "create <dictName> <inputFile> <delimiter> -- "
         << "create a new dictionary\n\tfrom an input file." << endl;
    cout << "merge <firstDict> <secondDict> <newName> -- " 
         << "merge firstDict and secondDict into a new dictionary "
         << "named newName" << endl;
    cout << "subtract dictName\n\t-- Remove all entries in " 
         << "dictName from the current dictionary."
         << "\n\tDestructively modifies the current dictionary." 
         << endl;
    }        
  }
 
  bool create(string &name, string &infile, string &delim) {
    // close any existing dict.
    delete(dict);
    dict = new lemur::dictionary::PDict();
    if (dict->open(name)) {
      cerr << "create: " << name << " exists. Specify a new name." << endl;
      delete(dict);
      dict = NULL;
      return false;
    }
    dict->create(name);
    // need a flag for counts as 3rd param here.
    if (!dict->read(infile, delim)) {
      cerr << "create: Unable to open " << infile << endl;
      delete(dict);
      dict = NULL;
      return false;

    }
    // close and reopen to commit toc/buffers.
    dict->close();
    dict->open(name);
    return true;
  }

  void open(string &name) {
    // close any existing dict.
    delete(dict);
    dict = new lemur::dictionary::PDict();
    if (!dict->open(name)) {
      cerr << "open: " << name << " failed." << endl;
      delete(dict);
      dict = NULL;
    }
  }

  void merge(string &firstname, string &secondname, string &newname) {
    // close any existing dict.
    delete(dict);
    dict = new lemur::dictionary::PDict();
    if (dict->open(newname)) {
      cerr << "merge: " << newname << " exists. Specify a new name." << endl;
      delete(dict);
      dict = NULL;
      return;
    }

    dict->create(newname);
    lemur::dictionary::PDict *first, *second;
    first = new lemur::dictionary::PDict();
    first->open(firstname);
    second = new lemur::dictionary::PDict();
    second->open(secondname);
    // define the semantics of this -- need api call.
    string key;
    lemur::dictionary::DictEntryVector *entries;
    first->startIteration();
    while((entries = first->nextTranslations(key))) {
      // iterate over entries and call add
      for (lemur::dictionary::DictEntryVector::iterator it = entries->begin();
           it != entries->end(); it++) {
        dict->add(key, *it);
      }
      delete(entries);
    }
    // duplicate entries will overwrite.
    second->startIteration();
    while((entries = second->nextTranslations(key))) {
      // iterate over entries and call add
      for (lemur::dictionary::DictEntryVector::iterator it = entries->begin();
           it != entries->end(); it++) {
        dict->add(key, *it);
      }
      delete(entries);
    }
    first->close();
    second->close();
    delete(first);
    delete(second);
    // normalize?
    dict->normalize();
  }

  void subtract(string &name) {
    lemur::dictionary::PDict *subdict = new lemur::dictionary::PDict();
    subdict->open(name);
    // define the semantics of this -- need api call.
    // probably ought to have an iterator for PDict.
    subdict->startIteration();
    string key;
    lemur::dictionary::DictEntryVector *entries;
    while((entries = subdict->nextTranslations(key))) {
      // iterate over entries and call remove
      for (lemur::dictionary::DictEntryVector::iterator it = entries->begin();
           it != entries->end(); it++) {
        //      cerr << "removing " << key << endl;
        dict->remove(key, *it);
      }
      delete(entries);
    }
    subdict->close();
    delete(subdict);
    // normalize?
    dict->normalize();
  }

  void dump(string &name, string &delim) {
    dict->write(name, delim);
  }

  void close() {
    delete(dict);
    dict = NULL;
  }

  void normalize() {
    dict->normalize();
  }

  void stats() {
    int su = dict->getSourceCount();
    int tu = dict->getTargetCount();
    int n =  dict->getNumPairs();
    const string &name = dict->getName();
    cout << name << "\n|V| = " << n << "\n|S| = " << su  << "\n|T| = "
         << tu << endl;
  }

  void lookup(string &term) {
    lemur::dictionary::DictEntryVector *xlates = dict->getTranslations(term);
    if (xlates == NULL) return;
    string delim(";"); // this should be user specified.
    for (lemur::dictionary::DictEntryVector::iterator it = xlates->begin(); 
         it != xlates->end(); it++) {
      cout << term << delim << (*it).toString(delim) << endl;
    }
    delete(xlates);
  }
  
private:  
  lemur::dictionary::PDict *dict;
  Stemmer *sourceStemmer, *targetStemmer;
  Stopper *sourceStopper, *targetStopper;
  
};

int main(int argc, char *argv[]) {
  CommandInterp interpretor;
  if (argc == 1) { // add params for source/target stem and stop options.
    // we're in interactive mode
    string cmd;
    do {
      cout << "Command: ";
      cout.flush();
      cin >> cmd;
    } while (interpretor.process(cmd));
  } else {
    interpretor.process(argc, argv);
  }
  return 0;
}
