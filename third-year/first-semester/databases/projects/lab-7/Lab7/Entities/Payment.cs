using System.ComponentModel.DataAnnotations;

namespace Lab7.Entities;

public class Payment
{
    [Key]
    public int Id { get; set; }
    
    public required decimal Sum { get; set; }
    
    public required DateOnly DateOfPayment { get; set; }
    
    [MaxLength(50)]
    public required string PaymentType { get; set; }
    
    public required Bill Bill { get; set; }
    
    public required Lodger Lodger { get; set; }
}