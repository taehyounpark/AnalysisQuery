from functools import reduce
import ROOT

# Global variable to hold the LazyFlow instance
configured_instance = None

def configure(class_name, member_variables: dict = {}):

    global configured_instance  # Use global to modify the global variable

    if configured_instance is not None:
        raise RuntimeError("An Analysis instance is already configured.")

    def set_members(obj, attr_dict):
        """
        Recursively sets attributes based on a nested dictionary.
        """
        for key, val in attr_dict.items():
            # If the value is a dictionary, we need to recurse further
            if isinstance(val, dict):
                # Get the nested object (attribute)
                nested_obj = getattr(obj, key)
                # Recursively set attributes on this nested object
                set_members(nested_obj, val)
            else:
                # Base case: we reached the final key-value pair, set the attribute
                setattr(obj, key, val)

    try:
        # Navigate through the nested classes to get the target class
        ana_klass = reduce(lambda x, y: getattr(x, y, None), class_name.split('::'), ROOT)

        if ana_klass is None:
            raise AttributeError(f"Class '{class_name}' not found in ROOT namespace.")
        
        # Instantiate the class
        ana = ana_klass()

        # Set attributes
        set_members(ana, member_variables)
        
        # Set the global configured_instance
        configured_instance = ana
        
    except Exception as e:
        print(f"Error: {e}")
        
class LazyFlow():

    def __init__(self, name : str):
        self._name = name
        self._dataflow = None
        self._dataset = None
        self._columns = {}
        self._selections = {}
        self._queries = {}

    def read(self, columns: dict = None):
        for column_name, column_spec in columns.items():
            pass
        pass

    def define(self, columns: dict = None):
        for column_name, column_spec in columns.items():
            pass
        pass

    def select(self, selections: dict = None):
        for selection_name, selection_spec in selections.items():
            pass
        pass

    def query(self, queries: dict = None):
        for query_name, query_spec in queries.items():
            pass
        pass
    
    def output(self, file):
        pass
    
class LazyNode():

    def __init__(self, action):
        self,_action = action