class Simulator:
    def __init__(self):
        self.__time = 0

    def step(self):
        pass


class Product:
    def __init__(self, name: str):
        self.name: str = name


class StatsCounter:
    def __init__(self):
        self.__eatenAmount = 0
        self.__trashedAmount = 0

    def increase_eaten_counter(self):
        self.__eatenAmount += 1

    def increase_trashed_counter(self):
        self.__trashedAmount += 1


class Head:
    def __init__(self, interval: int, allowed_products: list[Product], storage: Storage):
        self.__interval: int = interval
        self.__allowed_products: list[Product] = allowed_products
        self.__storage: Storage = storage

    def step(self, time: int):
        if time % self.__interval == 0:
            


class Storage:
    def __init__(self, stats_counter: StatsCounter):
        self.__items: list[Product] = []
        self.__stats_counter: StatsCounter = stats_counter

    def add_item(self):
        if len(self.__items) >= 10:
            self.__stats_counter.increase_trashed_counter()

    def contains(self, product: Product):
        return len([x for x in self.__items if x.name == product.name]) != 0

    def remove_product(self, product: Product):
        self.__items.remove(product)


class Factory:
    def __init__(self, interval: int, product: Product):
        self.__interval: int = interval
        self.__product: Product = product

    def step(self, time: int):
        if
