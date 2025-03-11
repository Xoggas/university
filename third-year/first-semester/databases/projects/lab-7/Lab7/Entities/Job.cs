using System.ComponentModel.DataAnnotations;

namespace Lab7.Entities;

public class Job
{
    [Key]
    public int Id { get; set; }
    
    [MaxLength(50)]
    public required string Name { get; set; }
    
    public required DateOnly CompletionDate { get; set; }
    
    public required decimal TotalCost { get; set; }
    
    public required Worker Worker { get; set; }
    
    public required Lodger Lodger { get; set; }
}