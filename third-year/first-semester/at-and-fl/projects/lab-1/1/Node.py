class Node:
    def __init__(self, value: str, children=None):
        self.value = value
        self.children = children

    def get_tree_representation(self):
        if self.children:
            return f"{self.value}({''.join([x.get_tree_representation() for x in self.children])})"
        return f"{self.value}"

    def get_formatted_tree_representation(self, tabs=0):
        if self.children:
            return f"\n{'\t' * tabs}{tabs}. {self.value}{''.join([x.get_formatted_tree_representation(tabs + 1) for x in self.children])}"
        return f"\n{'\t' * tabs}{tabs}. {self.value}"

    def get_plain_representation(self):
        if self.children:
            return ''.join([x.get_plain_representation() for x in self.children])
        return self.value

    def find_first_non_terminal_node_left(self):
        return self.find_node_left(lambda x: x.value.isupper())

    def find_non_terminal_node_by_value_left(self, value):
        return self.find_node_left(lambda x: x.value == value)

    def find_first_non_terminal_node_right(self):
        return self.find_node_right(lambda x: x.value.isupper())

    def find_non_terminal_node_by_value_right(self, value):
        return self.find_node_right(lambda x: x.value == value)

    def find_node_left(self, predicate):
        if self.children:
            stack = []
            for i in self.children:
                stack.append(i.find_node_left(predicate))
            stack = [x for x in stack if x is not None]
            if stack:
                return stack[0]
        elif predicate(self):
            return self
        else:
            return None

    def find_node_right(self, predicate):
        if self.children:
            stack = []
            for i in self.children[::-1]:
                stack.append(i.find_node_right(predicate))
            stack = [x for x in stack if x is not None]
            if stack:
                return stack[0]
        elif predicate(self):
            return self
        else:
            return None

    def __str__(self):
        return str(self.value)
