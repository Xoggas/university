using Lab7.Entities;
using Microsoft.EntityFrameworkCore;

namespace Lab7.Data;

public sealed class AppDbContext : DbContext
{
    public DbSet<Worker> Workers { get; set; }
    public DbSet<Job> Jobs { get; set; }
    public DbSet<Lodger> Lodgers { get; set; }
    public DbSet<Bill> Bills { get; set; }
    public DbSet<Payment> Payments { get; set; }

    protected override void OnConfiguring(DbContextOptionsBuilder options)
    {
        options.UseSqlite("Data Source=app.db")
            .UseSeeding((context, _) => SeedDatabase(context));
    }

    private static void SeedDatabase(DbContext context)
    {
        if (context.Set<Worker>().Any())
        {
            return;
        }

        var workers = new List<Worker>
        {
            new() { FullName = "Иванов Иван", Specialization = "Сантехник", Phone = "+79001112233" },
            new() { FullName = "Петров Петр", Specialization = "Электрик", Phone = "+79004445566" },
            new() { FullName = "Сидоров Сидор", Specialization = "Плотник", Phone = "+79007778899" },
            new() { FullName = "Ковалев Артем", Specialization = "Маляр", Phone = "+79001234567" },
            new() { FullName = "Фёдоров Алексей", Specialization = "Кровельщик", Phone = "+79005556677" },
            new() { FullName = "Григорьев Виктор", Specialization = "Стекольщик", Phone = "+79009887766" }
        };
        
        context.Set<Worker>().AddRange(workers);

        var lodgers = new List<Lodger>
        {
            new() { Name = "Алексей Смирнов", Address = "ул. Ленина, д. 10", Phone = "+79101112233" },
            new() { Name = "Мария Кузнецова", Address = "ул. Пушкина, д. 5", Phone = "+79102223344" },
            new() { Name = "Дмитрий Орлов", Address = "пр. Гагарина, д. 15", Phone = "+79103334455" },
            new() { Name = "Елена Васильева", Address = "ул. Советская, д. 20", Phone = "+79104445566" },
            new() { Name = "Олег Захаров", Address = "ул. Мира, д. 25", Phone = "+79105556677" },
            new() { Name = "Татьяна Белова", Address = "ул. Гоголя, д. 30", Phone = "+79106667788" }
        };
        
        context.Set<Lodger>().AddRange(lodgers);

        context.SaveChanges();

        var jobs = new List<Job>
        {
            new()
            {
                Name = "Замена труб", CompletionDate = DateOnly.FromDateTime(DateTime.Now.AddDays(-10)),
                TotalCost = 5000, Worker = workers[0], Lodger = lodgers[0]
            },
            new()
            {
                Name = "Прокладка проводки", CompletionDate = DateOnly.FromDateTime(DateTime.Now.AddDays(-5)),
                TotalCost = 7000, Worker = workers[1], Lodger = lodgers[1]
            },
            new()
            {
                Name = "Ремонт крыши", CompletionDate = DateOnly.FromDateTime(DateTime.Now.AddDays(-3)),
                TotalCost = 10000, Worker = workers[2], Lodger = lodgers[2]
            },
            new()
            {
                Name = "Покраска стен", CompletionDate = DateOnly.FromDateTime(DateTime.Now.AddDays(-1)),
                TotalCost = 3000, Worker = workers[3], Lodger = lodgers[3]
            },
            new()
            {
                Name = "Ремонт окон", CompletionDate = DateOnly.FromDateTime(DateTime.Now.AddDays(-7)),
                TotalCost = 4000, Worker = workers[5], Lodger = lodgers[4]
            }
        };
        
        context.Set<Job>().AddRange(jobs);

        var bills = new List<Bill>
        {
            new()
            {
                TotalCost = 5000, DateOfIssue = DateOnly.FromDateTime(DateTime.Now.AddDays(-9)), Status = false,
                Lodger = lodgers[0]
            },
            new()
            {
                TotalCost = 7000, DateOfIssue = DateOnly.FromDateTime(DateTime.Now.AddDays(-4)), Status = true,
                Lodger = lodgers[1]
            },
            new()
            {
                TotalCost = 10000, DateOfIssue = DateOnly.FromDateTime(DateTime.Now.AddDays(-2)), Status = false,
                Lodger = lodgers[2]
            },
            new()
            {
                TotalCost = 3000, DateOfIssue = DateOnly.FromDateTime(DateTime.Now), Status = false, Lodger = lodgers[3]
            },
            new()
            {
                TotalCost = 4000, DateOfIssue = DateOnly.FromDateTime(DateTime.Now.AddDays(-5)), Status = false,
                Lodger = lodgers[4]
            }
        };
        
        context.Set<Bill>().AddRange(bills);

        var payments = new List<Payment>
        {
            new()
            {
                Sum = 7000, DateOfPayment = DateOnly.FromDateTime(DateTime.Now.AddDays(-2)), PaymentType = "Карта",
                Bill = bills[1], Lodger = lodgers[1]
            },
            new()
            {
                Sum = 3000, DateOfPayment = DateOnly.FromDateTime(DateTime.Now), PaymentType = "Наличные",
                Bill = bills[3], Lodger = lodgers[3]
            }
        };
        
        context.Set<Payment>().AddRange(payments);

        context.SaveChanges();
    }
}