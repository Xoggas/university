using System.ComponentModel.DataAnnotations;

namespace Lab7.Entities;

public class Worker
{
    [Key]
    public int Id { get; set; }
    
    [Phone]
    [MaxLength(10)]
    public required string Phone { get; set; }
    
    [MaxLength(50)]
    public required string Specialization { get; set; }
    
    [MaxLength(100)]
    public required string FullName { get; set; }
}