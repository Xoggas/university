from Node import Node
from Rule import Rule


class Task1:
    def __init__(self, grammar_rules):
        self.root = Node('S')
        self.grammar_rules = grammar_rules
        self.steps = []
        self.step_index = 1

    def apply(self):
        while True:
            if not self.has_any_non_terminals():
                self.print_end_info()
                break
            else:
                self.print_info()
                rule, rule_index = self.get_rule_from_input()
                node = self.get_first_non_terminal_that_fits_rule(rule)
                if node is not None:
                    self.apply_rule_for(node, rule)
                    self.step_index += 1
                    self.steps.append(rule_index)
                else:
                    print('Невозможно применить выбранное правило ни к одному нетерминалу')

    def has_any_non_terminals(self):
        return self.root.find_first_non_terminal_node_left() is not None

    def get_first_non_terminal_that_fits_rule(self, rule):
        return self.root.find_non_terminal_node_by_value_left(rule.input)

    def print_info(self):
        print(f'### Шаг {self.step_index} ###')
        print(f'Промежуточная цепочка: {self.root.get_plain_representation()}')
        print('Можно применить: ')
        for (i, rule) in enumerate(self.grammar_rules):
            print(f'{i + 1}. {rule}')

    def print_end_info(self):
        print(f'### Шаг {self.step_index} ###')
        print(f'Терминальная цепочка: {self.root.get_plain_representation()}')
        print(f'Последовательность правил: {' '.join(map(str, [x + 1 for x in self.steps]))}')
        print(f'ЛСФ ДВ: {self.root.get_tree_representation()}')

    def get_rule_from_input(self):
        rule_index = int(input("Выберите правило: ")) - 1
        print()
        return self.grammar_rules[rule_index], rule_index

    @staticmethod
    def apply_rule_for(node, rule):
        node.children = [Node(x) for x in rule.output]


def main():
    output = Task1([
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
