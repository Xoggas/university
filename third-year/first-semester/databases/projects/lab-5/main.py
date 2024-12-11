import psycopg2


class DBService:
    def __init__(self, db_name, user, password, host='localhost', port=5432):
        try:
            self.connection = psycopg2.connect(
                dbname=db_name,
                user=user,
                password=password,
                host=host,
                port=port
            )
            self.cursor = self.connection.cursor()
        except psycopg2.Error as e:
            print(f"Ошибка подключения к базе данных: {e}")
            exit()

    def execute_query(self, query):
        try:
            self.cursor.execute(query)
            results = self.cursor.fetchall()
            return results
        except psycopg2.Error as e:
            return f"Ошибка выполнения запроса: {e}"

    def close(self):
        self.cursor.close()
        self.connection.close()


class UIService:
    def __init__(self, db_service):
        self.db_service = db_service
        self.queries = {
            1: {
                "description": "Список неплательщиков по возрастанию суммы долга",
                "query": """
                    SELECT 
                        l.full_name AS lodger_name,
                        l.phone,
                        b.total_cost AS debt_amount,
                        b.date_of_issue
                    FROM 
                        lodgers l
                    JOIN 
                        bills b ON l.id = b.issued_to
                    WHERE 
                        b.status = false
                    ORDER BY 
                        b.total_cost ASC;
                """
            },
            2: {
                "description": "Рейтинг исполнителей работ за промежуток времени",
                "query": """
                    SELECT 
                        w.full_name AS worker_name,
                        COUNT(j.id) AS jobs_completed
                    FROM 
                        workers w
                    JOIN 
                        jobs j ON w.id = j.worker_id
                    WHERE 
                        j.completion_date BETWEEN '2024-01-01' AND '2024-06-30'
                    GROUP BY 
                        w.full_name
                    ORDER BY 
                        jobs_completed DESC;
                """
            },
            3: {
                "description": "Список всех работ с их стоимостью и именами жильцов",
                "query": """
                    SELECT 
                        j.job_name,
                        j.total_cost,
                        l.full_name AS lodger_name
                    FROM 
                        jobs j
                    JOIN 
                        lodgers l ON j.lodger_id = l.id;
                """
            },
            4: {
                "description": "Общая сумма задолженности каждого жильца",
                "query": """
                    SELECT 
                        l.full_name AS lodger_name,
                        SUM(b.total_cost) AS total_debt
                    FROM 
                        lodgers l
                    JOIN 
                        bills b ON l.id = b.issued_to
                    WHERE 
                        b.status = false
                    GROUP BY 
                        l.full_name
                    ORDER BY 
                        total_debt DESC;
                """
            },
            5: {
                "description": "Список платежей с методами оплаты и датами",
                "query": """
                    SELECT 
                        p.id,
                        p.total_cost,
                        p.payment_method,
                        p.date_of_payment
                    FROM 
                        payments p;
                """
            }
        }

    def show_menu(self):
        print("Доступные запросы:")
        for index, query_info in self.queries.items():
            print(f"{index}. {query_info['description']}")

    def get_user_choice(self):
        try:
            choice = int(input("Введите индекс запроса (0 для выхода): "))
            return choice
        except ValueError:
            print("Ошибка: Введите целое число.")
            return None

    def display_results(self, results):
        if isinstance(results, str):
            print(results)
        elif results:
            for row in results:
                print(row)
        else:
            print("Запрос не вернул результатов.")

    def run(self):
        while True:
            self.show_menu()
            choice = self.get_user_choice()
            if choice == 0:
                print("Выход из программы.")
                break
            elif choice in self.queries:
                query = self.queries[choice]["query"]
                print(f"\nВыполняется запрос: {self.queries[choice]['description']}\n")
                results = self.db_service.execute_query(query)
                self.display_results(results)
                print("\n" + "-" * 50 + "\n")
            else:
                print("Ошибка: Некорректный индекс запроса.\n")


def main():
    db_name = "postgres"
    user = "postgres"
    password = "12345678"
    host = "localhost"
    port = 5432

    db_service = DBService(db_name, user, password, host, port)
    ui_service = UIService(db_service)

    try:
        ui_service.run()
    finally:
        db_service.close()


if __name__ == "__main__":
    main()
