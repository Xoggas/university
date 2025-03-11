using System.ComponentModel.DataAnnotations;

namespace Lab7.Entities;

public class Bill
{
    [Key]
    public int Id { get; set; }
    
    public required decimal TotalCost { get; set; }
    
    public required DateOnly DateOfIssue { get; set; }
    
    public bool Status { get; set; }
    
    public required Lodger Lodger { get; set; }
}