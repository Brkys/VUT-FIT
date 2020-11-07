@extends('layouts.app')

@section('title', 'Můj účet')

@section('navpanel')
	@parent
@endsection

@section('content')
<div style="padding-top: 100px"></div>
<div class="container" style="width: 80% !important; background-color: white !important; border-radius: 4 !important;">
	<form action="change-account" method="post">
		@csrf
	<h4 style="padding-top: 20px;">Jméno</h4>
	<div class="input-group mb-3">
	    <input type="text" class="form-control" placeholder="{{ $user[0]->full_name }}" aria-label="Celé jméno..." aria-describedby="basic-addon2" name="fullName">
	</div>
	<h6 style="padding-top: 20px">Přezdívka</h6>
	<div class="input-group mb-3">
	    <input type="text" class="form-control" placeholder="{{ $user[0]->name }}" aria-label="Přezdívka..." aria-describedby="basic-addon2" name="nickName">
	</div>
	<h6 style="padding-top: 20px">Úroveň účtu</h6>
	@if($_SESSION['permission'] === 5)
		Administrátor
	@elseif ($_SESSION['permission'] === 4)
		Don
	@elseif ($_SESSION['permission'] === 3)
		Consigliare 
	@elseif ($_SESSION['permission'] === 2)
		Caporegime - vyšší
	@elseif ($_SESSION['permission'] === 1)
		Caporegime- nižší
	@elseif ($_SESSION['permission'] === 0)
		Člen
	@else
		Uživatel bez rodiny
	@endif

	@if($_SESSION['permission'] < 3)		
		<h5 style="padding-top: 20px">Počet splněných kriminálních činností</h5>
		<h6 style="padding-top: 20px">{{ $count[0]->count }}</h6> 
	@endif	
	@if($_SESSION['permission'] === -1)
	<h4 style="padding-top: 20px;">Pozvánky</h4>
    <table class="table table-hover table-dark text-centered">
        <thead>
            <tr>
                <th scope="col" style="width: 60%;">Pozvánka od</th>
                <th scope="col" style="width: 20%;"></th>
                <th scope="col" style="width: 20%;"></th>
            </tr>
        </thead>
        <tbody>
		@if(!empty($invites))
			@foreach($invites as $invite)
            <tr>
                <td>{{$invite['JmenoFamilie']}}</td>

                <td>
					<form action="accept" method="post">
                        @csrf
						<input type="hidden" value="{{$invite['ID_Inv']}}" name="invID">
                        <input type="hidden" value="{{$invite['ID_Familie']}}" name="id">
                        <button type="submit" class="btn btn-primary" name="accept">Přijmout</button>
                	</form>
				</td>

				<td>
					<form action="decline" method="post">
                        @csrf
						<input type="hidden" value="{{$invite['ID_Inv']}}" name="invID">
                        <input type="hidden" value="{{$invite['ID_Familie']}}" name="id">
                        <button type="submit" class="btn btn-primary" name="decline">Odmítnout</button>
                	</form>
				</td>
            </tr>
			@endforeach
		@endif
        </tbody>
    </table>
    @endif
    <div style="padding-top: 20px"></div>
	<button type="submit" class="btn btn-primary btn-block" name="change-account">Uložit změny</button>
	<div style="padding-top: 20px"></div>
	</form>
</div>
@endsection