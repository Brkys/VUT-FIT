@extends('layouts.app')

@section('title', 'Administrace')

@section('navpanel')
	@parent
@endsection

@section('content')
<div style="padding-top: 100px"></div>
<div class="container" style="width: 80% !important; background-color: white !important; border-radius: 4 !important;">
	<form action="createNewFamilia" method="post">
    	@csrf
		<h4 style="padding-top: 20px;">Přidat familii</h4>
		<input class="form-control" id="myInput" type="text" placeholder="Jméno familie" name="familia_name">
		<h6 style="padding-top: 20px">Don</h6>
		<select class="form-control" id="sel1" name="don_id">
			@if(!empty($freeUsers))
				@foreach($freeUsers as $user)
		    		<option value="{{$user->id}}">{{$user->full_name}}</option>
		    	@endforeach
		    @endif
		</select>
		<div style="padding-top: 20px"></div>
		<button type="submit" class="btn btn-primary btn-block" name="createNewFamilia">Potvrdit</button>
	</form>
	<hr>
	<form action="createNewLand" onsubmit="return checkInput()" method="post" name="land_adding">
		@csrf
		<h4 style="padding-top: 20px;">Přidat území</h4>
		<input class="form-control" id="myInput" type="text" placeholder="Jméno území" name="land_name">
		<h6>Majitel (volitelné)</h6>
		<select class="form-control" id="sel1" name="land_owner">
		    <option value="NULL">Bez majitele</option>
			@if(!empty($familias))
				@foreach($familias as $familia)
		    		<option value="{{$familia->ID_Familie}}">{{$familia->JmenoFamilie}}</option>
		    	@endforeach
		    @endif
		</select>
		<h6 style="padding-top: 20px">Adresa</h6>
		<input class="form-control" id="myInput" type="text" placeholder="Adresa" name="land_address">
		<h6 style="padding-top: 20px">Rozloha v m^2</h6>
		<input class="form-control" id="myInput" type="text" placeholder="Rozloha" name="land_size"> <!-- check čisla -->
		<button type="submit" class="btn btn-primary btn-block" name="createNewLand">Potvrdit</button>
	</form>
	<div style="padding-top: 20px"></div>
	<hr>
	<form action="deleteFamilia" method="post">
		@csrf
		<h6 style="padding-top: 20px">Zrušit familii</h6>
		<select class="form-control" id="sel1" name="familia_id">
			@if(!empty($familias))
				@foreach($familias as $familia)
					@if($familia->ID_Dona !== NULL)
		    			<option value="{{$familia->ID_Familie}}">{{$familia->JmenoFamilie}}</option>
		    		@endif
		    	@endforeach
		    @endif
		</select>
	    <div style="padding-top: 20px"></div>
		<button type="submit" class="btn btn-primary btn-block" name="deleteFamilia">Potvrdit</button>
	</form>

	<script type="text/javascript">
	function checkInput()
	{
	  	var x=document.forms["land_adding"]["land_size"].value;
	  	if (isNaN(x)) 
	    {
	    	alert("Rozloha území musí být číslo!");
	    	return false;
		}
	}
	</script>
</div>
@endsection