from Node import Node
from Rule import Rule


class Task3:
    def __init__(self, grammar_rules):
        self.root = Node('S')
        self.grammar_rules = grammar_rules
        self.steps = []
        self.step_index = 1

    def apply(self):
        for index, rule in enumerate(self.grammar_rules):
            print(f'{index + 1}. {rule}')

        inputs = [int(x) - 1 for x in input('Введите последовательность шагов: ').split(',')]

        for i in inputs:
            rule = self.grammar_rules[i]
            node = self.get_node_that_fits_rule(rule)
            if node is None:
                print('Последовательность невозможно применить')
                return
            else:
                self.apply_rule_for(node, rule)
                self.step_index += 1
                self.steps.append(i)

        self.print_end_info()

        print('Последовательность можно применить')

    def get_node_that_fits_rule(self, rule):
        return self.root.find_non_terminal_node_by_value_left(rule.input)

    def print_end_info(self):
        print(f'Терминальная цепочка: {self.root.get_plain_representation()}')
        print(f'Последовательность правил: {' '.join(map(str, [x + 1 for x in self.steps]))}')
        print(f'ЛСФ ДВ: {self.root.get_tree_representation()}')
        print(f'ЛСФ ДВ (табы): {self.root.get_formatted_tree_representation()}')

    @staticmethod
    def get_rule_from_input(rules, rule_index):
        return rules[rule_index], rule_index

    @staticmethod
    def apply_rule_for(node, rule):
        node.children = [Node(x) for x in rule.output]


def main():
    output = Task3([
        Rule('S', ['a', 'S', 'S', 'a']),
        Rule('S', ['b', 'S', 'b']),
        Rule('S', ['c', 'A', 'b']),
        Rule('A', ['A', 'a']),
        Rule('A', ['B', 'a']),
        Rule('A', ['']),
        Rule('B', ['b', 'B']),
        Rule('B', ['a', 'A'])
    ])

    output.apply()


if __name__ == '__main__':
    main()
