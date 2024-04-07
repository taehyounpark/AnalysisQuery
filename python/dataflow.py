import ROOT
from ROOT import qty
import cppyy

df = qty.dataflow()

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

