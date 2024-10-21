import unittest
from Node import Node


class NodeTest(unittest.TestCase):
    def test_init(self):
        node = Node('A')
        self.assertEqual(node.value, 'A')

    def test_get_plain_representation(self):
        node = Node('S', [
            Node('a'),
            Node('S', [
                Node('b'),
                Node('S', [
                    Node('c'),
                    Node('A', [
                        Node('')
                    ]),
                    Node('b')
                ]),
                Node('b')
            ]),
            Node('S'),
            Node('a')
        ])
        self.assertEqual(node.get_plain_representation(), 'abcbbSa')

    def test_get_tree_representation(self):
        node = Node('S', [
            Node('a'),
            Node('S', [
                Node('b'),
                Node('S', [
                    Node('c'),
                    Node('A', [
                        Node('')
                    ]),
                    Node('b')
                ]),
                Node('b')
            ]),
            Node('S'),
            Node('a')
        ])
        self.assertEqual(node.get_tree_representation(), 'S(aS(bS(cA()b)b)Sa)')

    def test_find_node_should_return_existing_node(self):
        node = Node('S', [
            Node('a'),
            Node('S', [
                Node('b'),
                Node('S', [
                    Node('c'),
                    Node('A', [
                        Node('')
                    ]),
                    Node('b')
                ]),
                Node('b')
            ]),
            Node('S'),
            Node('a')
        ])
        self.assertIsNotNone(node.find_node_left(lambda x: x.value == 'S'))

    def test_find_node_should_return_None(self):
        node = Node('S', [
            Node('a'),
            Node('S', [
                Node('b'),
                Node('S', [
                    Node('c'),
                    Node('A', [
                        Node('')
                    ]),
                    Node('b')
                ]),
                Node('b')
            ]),
            Node('S'),
            Node('a')
        ])
        self.assertIsNone(node.find_node_left(lambda x: x.value == 'A'))

    def test_find_first_non_terminal_should_be_not_None(self):
        node = Node('S', [
            Node('a'),
            Node('S', [
                Node('b'),
                Node('S', [
                    Node('c'),
                    Node('A', [
                        Node('')
                    ]),
                    Node('b')
                ]),
                Node('b')
            ]),
            Node('S'),
            Node('a')
        ])
        self.assertIsNotNone(node.find_first_non_terminal_node_left())

    def test_find_first_non_terminal_should_return_itself(self):
        node = Node('S')
        self.assertEqual(node.find_first_non_terminal_node_left(), node)

    def test_find_first_non_terminal_should_return_None(self):
        node = Node('a')
        self.assertIsNone(node.find_first_non_terminal_node_left())


if __name__ == '__main__':
    unittest.main()
