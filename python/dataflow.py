import ROOT
from ROOT import queryosity as qty
import cppyy

# this module will ONLY be developed if and when users desire the feature,
# which will be to enable the dataflow interface to be invoked from Python.
# due to cppyy-backend (cling)'s limitations on ROOT side, automatic bindings
# do not work. Instead, a "manual" layer to translate Python(ic) function calls on the dataflow & lazy nodes to C++ using cling interpreter itself must be developed.
# note: this limitation does not exist in cppyy v3.0.0 and beyond.

# trigger queryosity to be loaded
df = qty.dataflow()
cppyy.cppdef('''
using dataflow = qty::dataflow;
namespace multithread = qty::multithread;
namespace dataset = qty::dataset;
namespace column = qty::column;
namespace selection = qty::selection;
namespace query = qty::query;
namespace systematic = qty::systematic;
'''
)

class LazyNode():
  def __init__(self, action):
    self._action = action

  def filter(self, expression=None):
    pass

  def weight(self, expression=None):
    pass

class LazyFlow():
  def __init__(self, multithread=False):
    cppyy.cppdef('''
    dataflow df;
    ''')
    self._df = cppyy.gbl.df
    self.columns = {}
    self.selections = {}
    self.queries = []

  def load(self):
    pass

  def read(self):
    pass

  def define(self):
    pass

  def filter(self):
    pass

  def weight(self):
    pass

  def get(self):
    pass

