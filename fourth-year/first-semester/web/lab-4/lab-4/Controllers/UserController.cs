using FluentValidation;
using Microsoft.AspNetCore.Mvc;
using Swashbuckle.AspNetCore.Annotations;
using Talksy.Api.Dtos.User;
using Talksy.Api.Services;

namespace Talksy.Api.Controllers;

/// <summary>
/// Управление пользователями (регистрация, авторизация, получение информации).
/// </summary>
[ApiController]
[Route("api/user")]
public class UserController : ControllerBase
{
    private readonly IUserService _userService;

    public UserController(IUserService userService)
    {
        _userService = userService;
    }

    /// <summary>
    /// Получить пользователя по имени пользователя (username).
    /// </summary>
    /// <param name="username">Имя пользователя.</param>
    /// <returns>Данные пользователя.</returns>
    [HttpGet("{username}")]
    [SwaggerOperation(Summary = "Получить пользователя по username",
        Description = "Возвращает данные пользователя по имени пользователя.")]
    [ProducesResponseType(typeof(UserDto), StatusCodes.Status200OK)]
    [ProducesResponseType(StatusCodes.Status404NotFound)]
    public async Task<ActionResult<UserDto>> GetUserByUsernameAsync(string username, IValidator<UserFindDto> validator)
    {
        var userFindDto = new UserFindDto
        {
            Username = username
        };

        var validationResult = await validator.ValidateAsync(userFindDto);

        if (validationResult.IsValid is false)
        {
            return BadRequest(validationResult.Errors);
        }

        var result = await _userService.GetUserByUsernameAsync(username);

        if (result.IsFailure)
        {
            return NotFound();
        }

        return Ok(result.Value);
    }

    /// <summary>
    /// Зарегистрировать нового пользователя.
    /// </summary>
    /// <param name="dto">Данные для регистрации.</param>
    /// <returns>Созданный пользователь.</returns>
    [HttpPost("register")]
    [SwaggerOperation(Summary = "Регистрация пользователя", Description = "Создает нового пользователя в системе.")]
    [ProducesResponseType(typeof(UserDto), StatusCodes.Status200OK)]
    [ProducesResponseType(StatusCodes.Status409Conflict)]
    public async Task<ActionResult<UserDto>> RegisterUserAsync(UserRegisterDto dto)
    {
        var result = await _userService.RegisterUserAsync(dto);

        if (result.IsFailure)
        {
            return Conflict(result.Error);
        }

        return Ok(result.Value);
    }

    /// <summary>
    /// Авторизовать пользователя и получить JWT-токен.
    /// </summary>
    /// <param name="dto">Логин и пароль.</param>
    /// <returns>JWT токен.</returns>
    [HttpPost("login")]
    [SwaggerOperation(Summary = "Авторизация пользователя",
        Description = "Возвращает JWT-токен при успешной авторизации.")]
    [ProducesResponseType(typeof(object), StatusCodes.Status200OK)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    public async Task<ActionResult<string>> AuthorizeUserAsync(UserLoginDto dto)
    {
        var result = await _userService.AuthorizeUserAsync(dto);

        if (result.IsFailure)
        {
            return Unauthorized();
        }

        return Ok(new
        {
            Token = result.Value
        });
    }
}