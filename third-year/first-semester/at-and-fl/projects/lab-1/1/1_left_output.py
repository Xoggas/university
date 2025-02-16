from operator import indexOf

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
            non_terminal_node = self.get_first_non_terminal()
            if not non_terminal_node:
                self.print_end_info()
                break
            else:
                rules = self.get_applicable_rules_for_node(non_terminal_node)
                self.print_info(rules)
                rule, rule_index = self.get_rule_from_input()
                self.apply_rule_for(non_terminal_node, rule)
                self.step_index += 1
                self.steps.append(rule_index)

    def get_first_non_terminal(self):
        return self.root.find_first_non_terminal_node_left()

    def get_applicable_rules_for_node(self, node):
        if not node:
            return None
        return [x for x in self.grammar_rules if x.input == node.value]

    def print_info(self, rules):
        print(f'### Шаг {self.step_index} ###')
        print(f'Промежуточная цепочка: {self.root.get_plain_representation()}')
        print('Можно применить: ')
        for (i, rule) in [(indexOf(self.grammar_rules, x) + 1, x) for i, x in enumerate(rules)]:
            print(f'{i}. {rule}')

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
