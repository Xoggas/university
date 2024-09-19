class Rule:
    def __init__(self, input, output):
        self.input = input
        self.output = output

    def __str__(self):
        return f'{self.input} -> {''.join(map(str, self.output))}'
