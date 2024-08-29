from functools import reduce
import ROOT

# Global variable to hold the analysis instance
configured_instance = None

def configure(class_name, member_variables: dict = {}):

    global configured_instance  # Use global to modify the global variable

    if configured_instance is not None:
        raise RuntimeError("An analysis instance is already configured.")

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