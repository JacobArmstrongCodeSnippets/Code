// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create the camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->bUsePawnControlRotation = false;

	//Create the camera
	MyCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("My Camera"));
	MyCamera->SetupAttachment(CameraBoom);
	MyCamera->bUsePawnControlRotation = false;

	//Set up initial character movement parameters
	//GetCharacterMovement()->bOrientRotationToMovement = true;
	//bUseControllerRotationYaw = false;
	CameraBoomTargetLength = 500;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (MyPlayerState)
	{
	case EPlayerStateEnum::PS_Jumping:
		if (GetCharacterMovement()->IsMovingOnGround())
			MyPlayerState = EPlayerStateEnum::PS_Walking;
		break;
	}

	//Quick hack for smoothly changing camera position; look into optimizing later
	CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, CameraBoomTargetLength, DeltaTime, 10);
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("VerticalAxis", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("HorizontalAxis", this, &AMyCharacter::MoveRight);

	PlayerInputComponent->BindAxis("MouseX", this, &AMyCharacter::RotateMouseControl);

	PlayerInputComponent->BindAction("SprintBtn", IE_Pressed, this, &AMyCharacter::SprintBtnPressed);
	PlayerInputComponent->BindAction("SprintBtn", IE_Released, this, &AMyCharacter::SprintBtnReleased);
	PlayerInputComponent->BindAction("CrouchBtn", IE_Pressed, this, &AMyCharacter::CrouchBtnPressed);
	PlayerInputComponent->BindAction("CrouchBtn", IE_Released, this, &AMyCharacter::CrouchBtnReleased);
	PlayerInputComponent->BindAction("JumpBtn", IE_Pressed, this, &AMyCharacter::JumpBtnPressed);
}

void AMyCharacter::MoveForward(float AxisValue)
{
	switch (MyPlayerState)
	{
	case EPlayerStateEnum::PS_Sprinting:
		if (AxisValue > 0)
		{
			//if forward is pressed, sprint forward
			AddMovementInput(GetActorForwardVector() * AxisValue);
		}
		else
		{
			//if vertical axis is released or pushed backward, stop sprinting
			//Set camera and walk speed values back to normal
			GetCharacterMovement()->MaxWalkSpeed = 600;
			CameraBoomTargetLength = 500;

			MyPlayerState = EPlayerStateEnum::PS_Walking;
		}
		break;
	default:
		//Move normally
		AddMovementInput(GetActorForwardVector() * AxisValue);
		break;
	}
}

void AMyCharacter::MoveRight(float AxisValue)
{
	switch (MyPlayerState)
	{
	case EPlayerStateEnum::PS_Sprinting:
		//don't add right/left movement while sprinting
		break;
	default:
		AddMovementInput(GetActorRightVector() * AxisValue);
		break;
	}
}

void AMyCharacter::RotateMouseControl(float AxisValue)
{
	AddControllerYawInput(AxisValue);
}

void AMyCharacter::SprintBtnPressed()
{
	switch (MyPlayerState)
	{
	//Toggle sprint on
	case EPlayerStateEnum::PS_Walking:
	case EPlayerStateEnum::PS_Crouching:
	case EPlayerStateEnum::PS_Prone:
		//Undo crouching if necessary
		if (MyPlayerState == EPlayerStateEnum::PS_Crouching)
			UnCrouch();

		//TODO: Undo prone if necessary

		//Adjust movement and camera variables
		GetCharacterMovement()->MaxWalkSpeed = 1200;
		CameraBoomTargetLength = 50;

		MyPlayerState = EPlayerStateEnum::PS_Sprinting;
		break;

	//Toggle sprint off
	case EPlayerStateEnum::PS_Sprinting:
		//Set camera and walk speed values back to normal
		GetCharacterMovement()->MaxWalkSpeed = 600;
		CameraBoomTargetLength = 500;

		MyPlayerState = EPlayerStateEnum::PS_Walking;
		break;

	//Do nothing if jumping
	case EPlayerStateEnum::PS_Jumping:
		break;
	}
}

void AMyCharacter::SprintBtnReleased()
{
	//Nothing done when sprint button is released
}

void AMyCharacter::CrouchBtnPressed()
{
	if (CanCrouch() == true)
	{
		Crouch();
		MyPlayerState = EPlayerStateEnum::PS_Crouching;
	}
}
void AMyCharacter::CrouchBtnReleased()
{
	if (MyPlayerState == EPlayerStateEnum::PS_Crouching)
	{
		UnCrouch();
		MyPlayerState = EPlayerStateEnum::PS_Walking;
	}
}

void AMyCharacter::JumpBtnPressed()
{
	Jump();
	if (MyPlayerState == EPlayerStateEnum::PS_Sprinting)
	{
		//Stop sprinting if necessary and set speed and cam values to normal
		GetCharacterMovement()->MaxWalkSpeed = 600;
		CameraBoomTargetLength = 500;
	}
	MyPlayerState = EPlayerStateEnum::PS_Jumping;
}