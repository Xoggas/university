using System.ComponentModel.DataAnnotations;

namespace Lab7.Entities;

public class Lodger
{
    [Key]
    public int Id { get; set; }
    
    [MaxLength(100)]
    public required string Name { get; set; }
    
    [MaxLength(100)]
    public required string Address { get; set; }
    
    [Phone]
    public required string Phone { get; set; }
}