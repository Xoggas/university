import psycopg2
import tkinter as tk
from tkinter import ttk, messagebox
from functools import partial


class DBService:
    def __init__(self, db_name, user, password, host="localhost", port=5432):
        try:
            self.conn = psycopg2.connect(
                dbname=db_name,
                user=user,
                password=password,
                host=host,
                port=port
            )
        except Exception as e:
            messagebox.showerror("Connection Error", str(e))
            self.conn = None

    def execute_query(self, query):
        if not self.conn:
            return [], []

        try:
            with self.conn.cursor() as cursor:
                cursor.execute(query)
                results = cursor.fetchall()
                columns = [desc[0] for desc in cursor.description]
                return results, columns
        except Exception as e:
            messagebox.showerror("Query Error", str(e))
            return [], []


class MainApp(tk.Frame):
    def __init__(self, root, db_service):
        super().__init__(root)
        self.db_service = db_service
        self.tree = None
        self.queries = {
            1: ("Неплательщики по возрастанию долга",
                """
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
                """),

            2: ("Рейтинг исполнителей работ",
                """
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
                """),

            3: ("Список всех работ с их стоимостью и именами жильцов",
                """
                SELECT 
                    j.job_name,
                    j.total_cost,
                    l.full_name AS lodger_name
                FROM 
                    jobs j
                JOIN 
                    lodgers l ON j.lodger_id = l.id;
                """),

            4: ("Общая сумма задолженности каждого жильца",
                """
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
                """),

            5: ("Список платежей с методами оплаты и датами",
                """
                SELECT 
                    p.id,
                    p.total_cost,
                    p.payment_method,
                    p.date_of_payment
                FROM 
                    payments p;
                """)
        }
        self.create_ui()

    def create_ui(self):
        self.pack(fill=tk.BOTH, expand=True)
        toolbar = tk.Frame(self)
        toolbar.pack(side=tk.TOP, fill=tk.X)

        for index, (label, _) in self.queries.items():
            btn = tk.Button(toolbar, text=label, command=partial(self.show_query_result, index))
            btn.pack(side=tk.LEFT, padx=4, pady=4)

        self.table_frame = tk.Frame(self)
        self.table_frame.pack(fill=tk.BOTH, expand=True)

    def show_query_result(self, query_index):
        query = self.queries[query_index][1]
        results, columns = self.db_service.execute_query(query)

        if self.tree:
            self.tree.destroy()

        if not results:
            return

        self.tree = ttk.Treeview(self.table_frame, columns=columns, show="headings")

        for col in columns:
            self.tree.heading(col, text=col)
            self.tree.column(col, anchor=tk.CENTER)

        for row in results:
            self.tree.insert("", tk.END, values=row)

        self.tree.pack(fill=tk.BOTH, expand=True)


if __name__ == "__main__":
    root = tk.Tk()
    root.title("Database GUI Application")
    root.geometry("800x600")

    db_service = DBService(db_name="postgres", user="postgres", password="12345678")
    app = MainApp(root, db_service)
    app.mainloop()
