using lab_3.Models;
using lab_3.Services;
using Microsoft.AspNetCore.Mvc;

namespace lab_3.Controllers;

[ApiController]
[Route("api/user")]
public sealed class UserController : ControllerBase
{
    private readonly UserService _userService;

    public UserController(UserService userService)
    {
        _userService = userService;
    }

    [HttpGet]
    public async Task<ActionResult<IEnumerable<User>>> GetAllUsersAsync()
    {
        return Ok(await _userService.GetUsersAsync());
    }

    [HttpGet("{id:guid}")]
    public async Task<ActionResult<User>> GetUserByIdAsync(Guid id)
    {
        var user = await _userService.GetUserById(id);

        if (user is null)
        {
            return NotFound();
        }

        return Ok(user);
    }
}