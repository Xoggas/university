class Node:
    def __init__(self, value: str, children=None):
        self.value = value
        self.children = children

    def get_tree_representation(self):
        if self.children:
            return f"{self.value}({''.join([x.get_tree_representation() for x in self.children])})"
        return f"{self.value}"

    def get_plain_representation(self):
        if self.children:
            return ''.join([x.get_plain_representation() for x in self.children])
        return self.value

    def find_first_non_terminal_node(self):
        return self.find_node(lambda x: x.value.isupper())

    def find_node(self, predicate):
        if self.children:
            stack = []
            for i in self.children:
                stack.append(i.find_node(predicate))
            stack = [x for x in stack if x is not None]
            if stack:
                return stack[0]
        elif predicate(self):
            return self
        else:
            return None

    def __str__(self):
        return str(self.value)
