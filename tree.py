from nltk.tree import *

# assign your output (generalied list of the syntax tree) to varaible text
text = "[P [D [int ][L [I [C [a ]][I [C [r ]][I [C [k ]][I [C [i ]]]]]][, ][L [I [C [a ]][I [C [s ]][I [C [h ]]]]][, ][L [I [C [s ]][I [C [r ]][I [C [i ]]]]]]]]][; ]]"


text = text.replace("(", "ob")    #in the syntax tree, 'ob' will display in place of '('
text = text.replace(")", "cb")    #in the syntax tree, 'cb' will display in place of ')'
text = text.replace("[", "(")
text = text.replace("]", ")")


tree = Tree.fromstring(text)
tree.pretty_print(unicodelines=True, nodedist=10)   

